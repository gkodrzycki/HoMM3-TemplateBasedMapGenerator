#include "./TerrainPlacer.hpp"

TerrainPlacer::TerrainPlacer(Map &map) : map(map) {}

vector<vector<int3>> TerrainPlacer::collectObstacleComponents() {
    int width  = map.getWidth();
    int height = map.getHeight();

    auto passable = [&](const int3 &pos) {
        auto tile = map.getTile(pos);
        return tile && tile->isTileType("OBb");
    };

    auto neighbours = [&](const int3 &pos) {
        std::array<int3, 4> out;
        for (int i = 0; i < 4; i++)
            out[i] = pos + directions4[i];
        return out;
    };

    vector<vector<bool>> visited(width, vector<bool>(height, false));
    vector<vector<int3>> components;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (visited[x][y])
                continue;

            int3 start(x, y, 0);
            if (!passable(start))
                continue;

            auto component = bfs_claim_component(width, height, start, neighbours, passable);
            for (const auto &p : component) {
                visited[p.x][p.y] = true;
            }

            if (!component.empty()) {
                components.push_back(component);
            }
        }
    }

    return components;
}

vector<int3> TerrainPlacer::getTemplateCoveredTiles(const Obstacle &templ, const int3 &anchor) {
    vector<int3> coveredTiles;

    // Bounds check: realSize must have templ.getSize().y rows
    if ((int)templ.getRealSize().size() != templ.getSize().y) {
        cerr << "[TerrainPlacer] Warning: realSize row count mismatch for " << templ.getName()
             << " (expected " << templ.getSize().y << ", got " << templ.getRealSize().size()
             << ")\n";
        return coveredTiles;
    }

    for (int dy = 0; dy < templ.getSize().y; dy++) {
        // Bounds check: each row must have templ.getSize().x columns
        if ((int)templ.getRealSize()[dy].size() != templ.getSize().x) {
            cerr << "[TerrainPlacer] Warning: realSize column count mismatch for "
                 << templ.getName() << " row " << dy << " (expected " << templ.getSize().x
                 << ", got " << templ.getRealSize()[dy].size() << ")\n";
            return coveredTiles;
        }

        for (int dx = 0; dx < templ.getSize().x; dx++) {
            if (templ.getRealSize()[dy][dx] != '1')
                continue;

            int tileX = anchor.x - templ.getSize().x + dx + 1;
            int tileY = anchor.y - templ.getSize().y + dy + 1;
            coveredTiles.emplace_back(tileX, tileY, anchor.z);
        }
    }

    return coveredTiles;
}

bool TerrainPlacer::canPlaceTemplateAtAnchor(const Obstacle &templ, const int3 &anchor,
                                             const unordered_set<int3> &remaining) {
    // Check every tile inside the bounding box.
    // '1' tiles must be in the obstacle remaining set.
    // '0' (transparent visual) tiles must not overlap roads, buildings, guards or borders
    // - placing a large sprite there would look wrong and could block unintended paths.

    // Exclusion-group spacing check: reject if an already-placed obstacle of the same
    // group has its bounding box within minSpacing tiles of this one (Chebyshev metric).
    const string &group = templ.getExclusionGroup();
    if (!group.empty() && templ.getMinSpacing() > 0) {
        auto it = placedByExclusionGroup.find(group);
        if (it != placedByExclusionGroup.end()) {
            int newLeft  = anchor.x - templ.getSize().x + 1;
            int newTop   = anchor.y - templ.getSize().y + 1;
            int newRight = anchor.x;
            int newBot   = anchor.y;
            for (const auto &[existAnchor, existSize] : it->second) {
                int exLeft  = existAnchor.x - existSize.x + 1;
                int exTop   = existAnchor.y - existSize.y + 1;
                int exRight = existAnchor.x;
                int exBot   = existAnchor.y;
                int sepX    = max({0, exLeft - newRight, newLeft - exRight});
                int sepY    = max({0, exTop - newBot, newTop - exBot});
                if (sepX + sepY < templ.getMinSpacing())
                    return false;
            }
        }
    }

    // Bounds check: realSize must match size
    if ((int)templ.getRealSize().size() != templ.getSize().y) {
        return false;
    }

    for (int dy = 0; dy < templ.getSize().y; dy++) {
        if ((int)templ.getRealSize()[dy].size() != templ.getSize().x) {
            return false;
        }

        for (int dx = 0; dx < templ.getSize().x; dx++) {
            int tileX = anchor.x - templ.getSize().x + dx + 1;
            int tileY = anchor.y - templ.getSize().y + dy + 1;
            int3 tilePos(tileX, tileY, anchor.z);

            auto tile = map.getTile(tilePos);
            if (!tile)
                return false;

            if (templ.getRealSize()[dy][dx] == '1') {
                // Must be an unplaced obstacle tile
                if (remaining.find(tilePos) == remaining.end())
                    return false;
            } else {
                // Transparent tile - must not sit on anything that matters:
                // 'T'=TAKEN (building), 'R'=RESERVED, 'r'=ROAD, 'o'=OCCUPIED, 'G'=GUARD
                // B/b/X (border, obstacle body) are acceptable under transparent regions.
                if (tile->isTileType("TRroG"))
                    return false;
            }
        }
    }
    return true;
}

void TerrainPlacer::loadObstacleInfo() {
    obstacleInfoLoaded = true; // set first so a failed load doesn't retry forever

    ifstream file("./stats/obstacleInfo.json");
    if (!file.is_open()) {
        cerr << "[TerrainPlacer] Warning: could not open stats/obstacleInfo.json, "
             << "falling back to built-in obstacles.\n";
        return;
    }

    json jsonData;
    try {
        file >> jsonData;
    } catch (const exception &e) {
        cerr << "[TerrainPlacer] Warning: failed to parse obstacleInfo.json: " << e.what() << "\n";
        return;
    }

    for (const auto &item : jsonData) {
        string name = item.at("name").get<string>();

        int3 size(1, 1, 0);
        if (item.contains("size")) {
            string sizeStr = item.at("size").get<string>();
            auto comma     = sizeStr.find(',');
            if (comma != string::npos) {
                size.x = stoi(sizeStr.substr(0, comma));
                size.y = stoi(sizeStr.substr(comma + 1));
            }
        }

        vector<string> realSize;
        if (item.contains("realSize")) {
            for (const auto &row : item.at("realSize"))
                realSize.push_back(row.get<string>());
        } else {
            for (int i = 0; i < size.y; i++)
                realSize.push_back(string(size.x, '1'));
        }

        Obstacle templ(name, int3(0, 0, 0), "Obstacle", size, int3(0, 0, 0), realSize);

        // Apply prefix-based exclusion group defaults (Lake/Crater/LAva Flow share one group so
        // neither type is placed near the other).
        static const std::map<string, std::pair<string, int>> prefixExclusion = {
            {"Lake", {"Lake", 14}}, {"Crater", {"crater", 16}}, {"Lava Flow", {"crater", 13}}};
        {
            size_t cut    = name.find_first_of("_0123456789");
            string prefix = name.substr(0, cut);

            if (auto pit = prefixExclusion.find(prefix); pit != prefixExclusion.end()) {
                // Unpack the pair into nicely named variables!
                const auto &[group, distance] = pit->second;

                templ.setExclusionGroup(group);
                templ.setMinSpacing(distance);
            }
        }

        for (const auto &terrainVal : item.at("terrains")) {
            string terrain = terrainVal.get<string>();
            cachedObstaclesByTerrain[terrain].push_back(templ);
        }
    }

    // Sort each terrain's list by tile count descending - try to pack largest first
    for (auto &[terrain, templates] : cachedObstaclesByTerrain) {
        sort(templates.begin(), templates.end(), [](const Obstacle &a, const Obstacle &b) {
            return a.getTileCount() > b.getTileCount();
        });
    }
}

const vector<Obstacle> &TerrainPlacer::getObstaclesForTerrain(const string &terrain) {
    if (!obstacleInfoLoaded)
        loadObstacleInfo();

    string lower = terrain;
    transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    auto it = cachedObstaclesByTerrain.find(lower);
    if (it != cachedObstaclesByTerrain.end())
        return it->second;

    // Fallback: use a generic 1x1 pine tree if the terrain has no entry
    static vector<Obstacle> fallback;
    if (fallback.empty()) {
        Obstacle obs("Pine Trees", int3(0, 0, 0), "Obstacle", int3(1, 1, 0), int3(0, 0, 0),
                     vector<string>{"1"});
        fallback.push_back(obs);
    }
    return fallback;
}

void TerrainPlacer::placeObstacleComponent(const vector<int3> &component) {
    unordered_set<int3> remaining(component.begin(), component.end());

    while (!remaining.empty()) {
        // Sort candidate anchors bottom-right first so the anchor formula
        // (which measures offsets from bottom-right) is most likely to reach
        // the full rectangular neighbourhood.
        vector<int3> anchors(remaining.begin(), remaining.end());
        sort(anchors.begin(), anchors.end(), [](const int3 &a, const int3 &b) {
            if (a.y != b.y)
                return a.y > b.y;
            return a.x > b.x;
        });

        bool placed = false;
        for (const auto &anchor : anchors) {
            auto tile = map.getTile(anchor);
            if (!tile)
                continue;

            const auto &templates = getObstaclesForTerrain(tile->getTerrain());
            for (const auto &templ : templates) {
                if (templ.getTileCount() > (int)remaining.size())
                    continue;

                if (!canPlaceTemplateAtAnchor(templ, anchor, remaining))
                    continue;

                // Pick a random variant from templates with the exact same shape
                vector<Obstacle> sameShape;
                for (const auto &t : templates) {
                    if (t.getSize() == templ.getSize() && t.getRealSize() == templ.getRealSize())
                        sameShape.push_back(t);
                }

                if (sameShape.empty())
                    continue;

                const Obstacle chosen = map.getRNG().getRandomFromVector(sameShape);

                auto obstaclePtr = make_shared<Obstacle>(chosen);
                obstaclePtr->setPosition(anchor);
                map.addObject(obstaclePtr);

                // Record in exclusion group for future spacing checks
                if (!chosen.getExclusionGroup().empty()) {
                    placedByExclusionGroup[chosen.getExclusionGroup()].emplace_back(
                        anchor, chosen.getSize());
                }

                for (const auto &coveredTile : getTemplateCoveredTiles(templ, anchor)) {
                    remaining.erase(coveredTile);
                    map.getTile(coveredTile)->setTileType(TileType::TILE_OBSTACLE_BODY);
                }

                placed = true;
                break;
            }

            if (placed)
                break;
        }

        if (placed)
            continue;

        // fall back to a 1x1 obstacle
        int3 fallbackPos = anchors.front();
        auto tile        = map.getTile(fallbackPos);
        string name      = "Pine Trees";
        if (tile) {
            const auto &templates = getObstaclesForTerrain(tile->getTerrain());
            vector<Obstacle> singles;
            for (const auto &t : templates)
                if (t.getTileCount() == 1)
                    singles.push_back(t);
            if (!singles.empty())
                name = map.getRNG().getRandomFromVector(singles).getName();
        }

        auto obstaclePtr = make_shared<Obstacle>(name, fallbackPos, "Obstacle", int3(1, 1, 0),
                                                 int3(0, 0, 0), vector<string>{"1"});
        map.addObject(obstaclePtr);

        remaining.erase(fallbackPos);
    }
}

void TerrainPlacer::generateNoise() {
    int width  = map.getWidth();
    int height = map.getHeight();

    float initialFillProbability = 0.32f;
    int birthLimit               = 4;
    int survivalLimit            = 2;
    int smoothingIterations      = 2;

    AutomataConfig config{width,      height,        initialFillProbability,
                          birthLimit, survivalLimit, smoothingIterations};
    CellularAutomata automata(config);
    automata.generate(this->map);
    auto grid = automata.getGrid();

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            auto tilePtr = map.getTile(int3(x, y, 0));
            if (tilePtr != nullptr) {
                if (grid[y * width + x]) {
                    if (tilePtr->isTileType("B"))
                        continue;
                    tilePtr->setTileType(TileType::TILE_OBSTACLE);
                }
            }
        }
    }

    int rectWidth  = 7;
    int rectHeight = 7;

    AutomataConfig smallRectConfig{width,      height,        initialFillProbability,
                                   birthLimit, survivalLimit, smoothingIterations};
    CellularAutomata automataSmallRect(smallRectConfig);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            auto tilePtr = map.getTile(int3(x, y, 0));
            if (tilePtr == nullptr)
                continue;

            if (isFreeRect(x, y, rectWidth, rectHeight)) {
                automataSmallRect.generateSmallRectangle(this->map, rectWidth, rectHeight, x, y);
                auto subGrid = automataSmallRect.getGrid();

                for (int dy = 0; dy < rectHeight; dy++) {
                    for (int dx = 0; dx < rectWidth; dx++) {
                        auto t = map.getTile(int3(x + dx, y + dy, 0));
                        if (t == nullptr)
                            continue;
                        if (subGrid[dy * rectWidth + dx]) {
                            t->setTileType(TileType::TILE_OBSTACLE);
                        }
                    }
                }
            }
        }
    }

    fillClosedRooms();
}

bool TerrainPlacer::isFreeRect(int startX, int startY, int w, int h) {
    for (int y = startY; y < startY + h; ++y) {
        for (int x = startX; x < startX + w; ++x) {
            auto tilePtr = map.getTile(int3(x, y, 0));
            if (tilePtr == nullptr || tilePtr->isTileType("BOb"))
                return false;
        }
    }
    return true;
}

void TerrainPlacer::fillClosedRooms() {
    int W = map.getWidth();
    int H = map.getHeight();

    auto passable   = [&](const int3 &pos) { return map.getTile(pos)->isTileType("F"); };
    auto neighbours = [&](const int3 &pos) {
        std::array<int3, 4> out;
        for (int i = 0; i < 4; i++)
            out[i] = pos + directions4[i];
        return out;
    };

    vector<vector<bool>> visited(W, vector<bool>(H, false));

    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            int3 pos(x, y, 0);
            if (!map.getTile(pos)->isTileType("F"))
                continue;
            if (visited[pos.x][pos.y])
                continue;

            auto component = bfs_claim_component(W, H, pos, neighbours, passable);
            if (component.size() <= 10) {
                for (const auto &p : component) {
                    map.getTile(p)->setTileType(TileType::TILE_OBSTACLE);
                }
            }
            for (const auto &p : component) {
                visited[p.x][p.y] = true;
            }
        }
    }
}

void TerrainPlacer::placeObstacles() {
    placedByExclusionGroup.clear();
    auto components = collectObstacleComponents();
    for (const auto &component : components) {
        placeObstacleComponent(component);
    }
}
