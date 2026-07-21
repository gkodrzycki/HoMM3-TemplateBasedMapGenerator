#include "./Map.hpp"

Map::Map(RNG &rng, TemplateInfo templateInfo) : rng(rng), templateInfo(templateInfo), searchCtx() {}

pair<int, int> Map::chooseMapSize(int minimumSize, int maximumSize) {
    vector<string> sizes;

    for (int i = minimumSize; i <= maximumSize; i++) {
        if (i == 1 || i == 2)
            sizes.push_back("S");
        else if (i == 4 || i == 8)
            sizes.push_back("M");
        else if (i == 9 || i == 18)
            sizes.push_back("L");
        else if (i == 16 || i == 32)
            sizes.push_back("XL");
    }

    if (sizes.empty()) {
        throw runtime_error("No valid map sizes available for the given range.");
    }

    string mapSize = rng.getRandomFromVector(sizes);

    if (mapSize == "S")
        return {36, 36};
    if (mapSize == "M")
        return {72, 72};
    if (mapSize == "L")
        return {108, 108};
    if (mapSize == "XL")
        return {144, 144};

    throw runtime_error("Invalid map size selected: " + mapSize);
}

Tile *Map::getTile(int3 pos) {
    if (pos.x < 0 || pos.x >= width || pos.y < 0 || pos.y >= height)
        return nullptr;

    return tileMap[pos.y][pos.x].get();
}

string Map::getZoneHash(int zoneID) { return hashMap[zoneID]; }
GroupSettingMap &Map::getGroupSettingMap() { return groupSettingMap; }
HashMap Map::getHashMap() { return hashMap; }
TemplateInfo Map::getTemplateInfo() { return templateInfo; }
RNG &Map::getRNG() { return rng; }
ZoneMap Map::getZoneMap() { return zoneMap; }
const TileMap &Map::getTileMap() { return tileMap; }
ObjectVector Map::getObjectVector() { return objectVector; }
RoadVector Map::getRoadVector() { return roadVector; }
CreatureVector Map::getCreatureVector() { return creatureVector; }
TreasureVector Map::getTreasureVector() { return treasureVector; }
PandoraBoxVector Map::getPandoraBoxVector() { return pandoraBoxVector; }
MonolithVector Map::getMonolithVector() { return monolithVector; }
GridSearchContext &Map::getSearchCtx() { return searchCtx; }
GuardValueMap Map::getGuardValueMap() { return guardValueMap; }

int Map::getWidth() { return width; }
int Map::getHeight() { return height; }

void Map::addHash(int zoneID, string hash) { this->hashMap[zoneID] = hash; }
void Map::addZone(shared_ptr<Zone> zone) { this->zoneMap[zone->getZoneID()] = zone; }
void Map::addObject(shared_ptr<Object> object) { this->objectVector.push_back(object); }
void Map::addRoad(shared_ptr<Road> road) { this->roadVector.push_back(road); }
void Map::addCreature(shared_ptr<Creature> creature) { this->creatureVector.push_back(creature); }
void Map::addTreasure(shared_ptr<Treasure> treasure) { this->treasureVector.push_back(treasure); }
void Map::addPandoraBox(shared_ptr<PandoraBox> pandoraBox) {
    this->pandoraBoxVector.push_back(pandoraBox);
}
void Map::addMonoliths(shared_ptr<Object> monolithFrom, shared_ptr<Object> monolithDest) {
    this->monolithVector.push_back({monolithFrom, monolithDest});
}
void Map::addGuardValue(int3 pos, int value) { this->guardValueMap[pos] = value; }

void Map::fixReachability() {
    // Iterate over all zones and check if they are reachable from the starting zone (the one with
    // the town) If not, change the terrain of some tiles to make them reachable This is a simple
    // implementation and can be improved by using a more sophisticated pathfinding algorithm
    auto zoneMap = getZoneMap();
    if (zoneMap.empty()) {
        return;
    }

    unordered_map<int3, int3> monoliths;
    for (const auto &[monolithFrom, monolithDest] : getMonolithVector()) {
        monoliths[monolithFrom->getPosition()] = monolithDest->getPosition();
        monoliths[monolithDest->getPosition()] = monolithFrom->getPosition();
    }

    for (const auto &[zoneID, zone] : zoneMap) {
        int3 center = zone->getCenter();

        int W = getWidth();
        int H = getHeight();

        auto neighbors = [&](const int3 &p) {
            vector<int3> outVec;
            for (int i = 0; i < 8; i++)
                outVec.push_back(p + directions8[i]);

            if (monoliths.find(p) != monoliths.end()) {
                outVec.push_back(monoliths[p]);
            }
            return outVec;
        };
        auto passable = [&](const int3 &p) {
            auto tile = getTile(p);
            if (!tile)
                return false;
            if (tile->getZoneID() != zoneID)
                return false;
            if (tile->isTileType("BT"))
                return false;
            return true;
        };
        auto cost = [&](const int3 &a, const int3 &b) {
            auto tile = getTile(b);
            if (tile->isTileType("FrRoG"))
                return 0;
            return a.distance2DMH(b) + 1;
        };
        auto &ctx = getSearchCtx();
        dijkstra_reachability(ctx, center, neighbors, passable, cost);

        auto objectVector   = getObjectVector();
        auto monolithVector = getMonolithVector();
        auto treasureVector = getTreasureVector();

        auto combinedVector = objectVector;
        combinedVector.insert(combinedVector.end(), treasureVector.begin(), treasureVector.end());
        for (const auto &[monolithFrom, monolithDest] : monolithVector) {
            combinedVector.push_back(monolithFrom);
            combinedVector.push_back(monolithDest);
        }
        for (const auto &[pos, value] : getGuardValueMap()) {
            auto guardObject = Object(pos, "Guard", int3(1, 1, 0));
            combinedVector.push_back(make_shared<Object>(guardObject));
        }

        map<int3, bool> clearedTiles;
        for (auto obj : combinedVector) {
            int3 pos = obj->getPosition();
            if (getTile(pos)->isTileType("T")) {
                pos = pos + obj->getEntryPoint();
            }
            if (reach_dist(ctx, pos.x, pos.y) == numeric_limits<int>::max()) {
                continue;
            }
            if (reach_dist(ctx, pos.x, pos.y) == 0) {
                continue;
            }
            int distance = reach_dist(ctx, pos.x, pos.y);
            while (distance != 0) {
                auto tile = getTile(pos);
                if (tile->isTileType("bO")) {
                    tile->setTileType(TileType::TILE_FREE);
                }
                clearedTiles[pos] = true;
                pos               = reach_parent(ctx, pos.x, pos.y);
                distance          = reach_dist(ctx, pos.x, pos.y);
            }
        }
        dijkstra_reachability(ctx, center, neighbors, passable, cost);

        for (int j = 0; j < H; j++) {
            for (int i = 0; i < W; i++) {
                int3 pos(i, j, 0);
                if (reach_dist(ctx, pos.x, pos.y) == numeric_limits<int>::max()) {
                    continue;
                }
                if (reach_dist(ctx, pos.x, pos.y) == 0) {
                    continue;
                }
                if (clearedTiles[pos] == true) {
                    continue;
                }

                if (getTile(pos)->isTileType("F")) {
                    getTile(pos)->setTileType(TileType::TILE_OBSTACLE);
                }
            }
        }
    }
}

void Map::initTiles() {
    pair<int, int> width_height =
        chooseMapSize(templateInfo.getMinimumSize(), templateInfo.getMaximumSize());

    width  = width_height.first;
    height = width_height.second;
    searchCtx.resize(width, height);
    tileMap.resize(height, vector<shared_ptr<Tile>>(width));

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            tileMap[i][j] = std::make_shared<Tile>();
        }
    }
}

void Map::initMap() { initTiles(); }

void Map::tester() {

    vector<vector<int>> grid(10, vector<int>(10, 0));

    grid[1][2] = 1;
    grid[7][6] = 1;

    vector<int3> freeTiles;
    vector<int3> placedSameObjects;
    for (int y = 0; y < 10; y++) {
        for (int x = 0; x < 10; x++) {
            if (grid[y][x] == 0) {
                freeTiles.push_back(int3(x, y, 0));
            } else if (grid[y][x] == 1) {
                placedSameObjects.push_back(int3(x, y, 0));
            }
        }
    }

    float param = 0.8f;
    if (const char *envName = std::getenv("TAU_PARAM")) {
        param = std::stof(envName);
    }
    auto bestPositions = findBestDistributedPositionDEBUG(freeTiles, placedSameObjects, {},
                                                          int3(4, 4, 0), param, -1, -1);

    for (const auto &bestPos : bestPositions) {
        grid[bestPos.y][bestPos.x] = 2;
    }
    cerr << "Grid after placing best distributed positions:\n";
    printTestGrid(grid);
}

void Map::printTestGrid(const vector<vector<int>> &grid) {
    if (grid.empty() || grid[0].empty()) {
        cerr << "Cannot print an empty test grid.\n";
        return;
    }

    string baseFileName = "test-distributed-position";

    if (const char *envName = std::getenv("GRID_OUT")) {
        baseFileName = envName;
    }

    const string htmlFile = baseFileName + ".html";
    const string pngFile  = baseFileName + ".png";

    ofstream out(htmlFile);

    if (!out.is_open()) {
        cerr << "Could not open file: " << htmlFile << "\n";
        return;
    }

    out << "<pre style='"
           "background: #FFFFFF;"
           "color: #383A42;"
           "font-family: monospace;"
           "font-size: 26px;"
           "font-weight: 600;"
           "line-height: 1.2;"
           "padding: 40px;"
           "display: inline-block;"
           "'>\n";

    for (const auto &row : grid) {
        for (int cell : row) {
            char symbol;
            string hexColor;
            string ansiColor;
            bool isBold = false;

            switch (cell) {
            case 0:
                symbol    = '.';
                ansiColor = GREEN;
                hexColor  = "#50A14F";
                break;

            case 1:
                symbol    = 'O';
                ansiColor = BLUE;
                hexColor  = "#4078F2";
                break;

            case 2:
                symbol    = 'X';
                ansiColor = RED + BOLD;
                hexColor  = "#E45649";
                isBold    = true;
                break;

            default:
                symbol    = '?';
                ansiColor = "";
                hexColor  = "#383A42";
                break;
            }

            // Terminal output
            if (ansiColor.empty()) {
                cerr << symbol;
            } else {
                printColor(ansiColor, symbol);
            }

            // HTML output
            out << "<span style='color: " << hexColor << ";";

            if (isBold) {
                out << " font-weight: 900;";
            }

            out << "'>" << symbol << "</span>";
        }

        out << "\n";
        cerr << "\n";
    }

    out << "</pre>\n";
    out.close();

    const string cmd = "google-chrome-stable --headless=new "
                       "--hide-scrollbars "
                       "--window-size=4000,4000 "
                       "--screenshot=\"" +
                       pngFile +
                       "\" "
                       "\"" +
                       htmlFile +
                       "\" "
                       "> /dev/null 2>&1 "
                       "&& mogrify -trim \"" +
                       pngFile + "\"";

    const int result = system(cmd.c_str());

    if (result != 0) {
        cerr << "Failed to generate screenshot: " << pngFile << "\n";
    }
}

void Map::generateMap() {
    // if (templateInfo.getDebug() > 0) {
    //     cerr << "==== SEED: " << rng.getOriginalSeed() << " ====\n";
    // }

    // tester();
    initMap();

    ZonePlacer zonePlacer(*this);
    zonePlacer.placeZones();
    zonePlacer.groupZones();

    if (templateInfo.getDebug() > 0) {
        cerr << "Zones placed...\n";
    }

    BorderPlacer borderPlacer(*this);
    borderPlacer.reserveBorderTiles();

    if (templateInfo.getDebug() > 0) {
        cerr << "Borders reserved...\n";
    }

    TerrainPlacer terrainPlacer(*this);
    terrainPlacer.generateNoise();

    if (templateInfo.getDebug() > 0) {
        cerr << "Noise generated...\n";
    }

    TownPlacer townPlacer(*this);
    townPlacer.placeTowns();

    if (templateInfo.getDebug() > 0) {
        cerr << "Towns placed...\n";
    }

    ConnectionPlacer connectionPlacer(*this);
    connectionPlacer.placeRoads();

    if (templateInfo.getDebug() > 0) {
        cerr << "Roads placed...\n";
    }
    connectionPlacer.createMonoliths();

    if (templateInfo.getDebug() > 0) {
        cerr << "Monoliths placed...\n";
    }

    ObjectPlacer objectPlacer(*this);
    // objectPlacer.placeBasicMines(); TODO: decide if we want to place basic mines standalone or
    // not
    objectPlacer.placeMines();

    if (templateInfo.getDebug() > 0) {
        cerr << "Mines placed...\n";
    }
    objectPlacer.placeMineResources();

    if (templateInfo.getDebug() > 0) {
        cerr << "Mine resources placed...\n";
    }
    // objectPlacer.placeTreasures();

    // if (templateInfo.getDebug() > 0) {
    //     cerr << "Treasures placed...\n";
    // }

    // GuardPlacer guardPlacer(*this);
    // guardPlacer.placeGuards();

    // if (templateInfo.getDebug() > 0) {
    //     cerr << "Guards placed, fixing reachability...\n";
    // }

    // fixReachability();

    // if (templateInfo.getDebug() > 0) {
    //     cerr << "Reachability fixed, placing borders and obstacles...\n";
    // }
    // terrainPlacer.placeObstacles();
    // borderPlacer.placeBorders();

    // if (templateInfo.getDebug() > 0) {
    //     printRealSizeMap();
    //     placeDebugObjects();
    // }
}

void Map::placeDebugObjects() {
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            int3 currentPos(x, y, 0);
            auto currentTilePtr = getTile(currentPos);

            if (currentTilePtr->isTileType("D")) {
                Obstacle obstacle("Pine Trees_Snow", currentPos, "Obstacle");
                auto obstaclePtr = make_shared<Obstacle>(obstacle);
                addObject(obstaclePtr);
            }
        }
    }
}

void Map::fixNeighbourTiles(const int3 &pos, const int3 &size, const vector<string> &realSize,
                            int zoneID, const int3 &offset) {
    auto zoneMap       = getZoneMap();
    string zoneTerrain = zoneMap[zoneID]->getTerrain();

    for (int dx = 0; dx < size.x + offset.x; dx++) {
        for (int dy = 0; dy < size.y + offset.y; dy++) {
            int3 tilePos(pos.x - size.x + dx + 1, pos.y - size.y + dy + 1, pos.z);
            auto tilePtr = getTile(tilePos);

            if (tilePtr == nullptr)
                continue;

            if (offset.x >= 1 && offset.y >= 1 &&
                (dx == size.x + offset.x - 1 || dy == size.y + offset.y - 1)) {
                tilePtr->setTileType(TileType::TILE_RESERVED);
            } else if (dx < size.x && dy < size.y && realSize[dy][dx] == '1') {
                tilePtr->setTileType(TileType::TILE_TAKEN);
            }
            tilePtr->setZoneID(zoneID);
            tilePtr->setTerrain(zoneTerrain);
        }
    }
}

bool Map::checkPlacementConflict(const int3 &pos, const int3 &size, const string &types,
                                 const int3 &offset, bool debug) {
    auto zoneMap = getZoneMap();

    if (debug) {
        cerr << "debugging checkplacement conflict\n";
    }

    for (int dx = 0; dx < size.x + offset.x; dx++) {
        for (int dy = 0; dy < size.y + offset.y; dy++) {
            int3 tilePos(pos.x - size.x + dx + 1, pos.y - size.y + dy + 1, pos.z);
            auto tilePtr = getTile(tilePos);

            if (tilePtr == nullptr)
                return true;

            if (tilePos == getZoneMap()[tilePtr->getZoneID()]->getCenter()) {
                return true;
            }

            if (debug) {
                cerr << tilePos.toString() << "\n";
            }

            if (tilePtr->isTileType(types)) {
                return true;
            }
        }
    }
    return false;
}

vector<int3> Map::findBestDistributedPositionDEBUG(const vector<int3> &freeTiles,
                                                   const vector<int3> &placedSameObjects,
                                                   const vector<int3> &placedAllObjects,
                                                   const int3 &zoneCenter, float tolerance,
                                                   int minDistanceRelative, int minDistanceTotal) {
    if (freeTiles.empty())
        return {int3(-1, -1, -1)};

    if (placedSameObjects.empty() && placedAllObjects.empty()) {
        int centerThresholdSq = 8;

        vector<int3> validCandidates;
        for (const auto &pos : freeTiles) {
            int dx     = pos.x - zoneCenter.x;
            int dy     = pos.y - zoneCenter.y;
            int distSq = dx * dx + dy * dy;

            if (distSq <= centerThresholdSq) {
                validCandidates.push_back(pos);
            }
        }
        if (validCandidates.empty()) {
            return {int3(-1, -1, -1)};
        }

        return validCandidates;
    }

    int maxMinDistSqRelative = -1;
    int maxMinDistSqTotal    = -1;
    vector<pair<int3, int>> tileDistances;
    tileDistances.reserve(freeTiles.size());

    for (const auto &pos : freeTiles) {
        int minDistToAnyObjectSqRelative = numeric_limits<int>::max();
        int minDistToAnyObjectSqTotal    = numeric_limits<int>::max();

        for (const auto &placedPos : placedSameObjects) {
            int dx     = pos.x - placedPos.x;
            int dy     = pos.y - placedPos.y;
            int distSq = dx * dx + dy * dy;
            if (distSq < minDistToAnyObjectSqRelative) {
                minDistToAnyObjectSqRelative = distSq;
            }
        }

        for (const auto &placedPos : placedAllObjects) {
            int dx     = pos.x - placedPos.x;
            int dy     = pos.y - placedPos.y;
            int distSq = dx * dx + dy * dy;
            if (distSq < minDistToAnyObjectSqTotal) {
                minDistToAnyObjectSqTotal = distSq;
            }
        }

        if (minDistanceTotal > 0 &&
            minDistToAnyObjectSqTotal < minDistanceTotal * minDistanceTotal) {
            continue;
        }

        tileDistances.push_back({pos, minDistToAnyObjectSqRelative});

        if (minDistToAnyObjectSqTotal > maxMinDistSqTotal) {
            maxMinDistSqTotal = minDistToAnyObjectSqTotal;
        }

        if (minDistToAnyObjectSqRelative > maxMinDistSqRelative) {
            maxMinDistSqRelative = minDistToAnyObjectSqRelative;
        }
    }

    if (minDistanceRelative > 0) {
        if (maxMinDistSqRelative < minDistanceRelative * minDistanceRelative) {
            return {int3(-1, -1, -1)};
        }
    }

    if (minDistanceTotal > 0) {
        if (maxMinDistSqTotal < minDistanceTotal * minDistanceTotal) {
            return {int3(-1, -1, -1)};
        }
    }
    int thresholdSq = static_cast<int>(maxMinDistSqRelative * (tolerance * tolerance));

    vector<int3> validCandidates;
    for (const auto &item : tileDistances) {
        if (item.second >= thresholdSq) {
            validCandidates.push_back(item.first);
        }
    }
    return validCandidates;
}

int3 Map::findBestDistributedPosition(const vector<int3> &freeTiles,
                                      const vector<int3> &placedSameObjects,
                                      const vector<int3> &placedAllObjects, const int3 &zoneCenter,
                                      float tolerance, int minDistanceRelative,
                                      int minDistanceTotal) {
    if (freeTiles.empty())
        return int3(-1, -1, -1);

    if (placedSameObjects.empty() && placedAllObjects.empty()) {
        int centerThresholdSq = 8;

        vector<int3> validCandidates;
        for (const auto &pos : freeTiles) {
            int dx     = pos.x - zoneCenter.x;
            int dy     = pos.y - zoneCenter.y;
            int distSq = dx * dx + dy * dy;

            if (distSq <= centerThresholdSq) {
                validCandidates.push_back(pos);
            }
        }
        if (validCandidates.empty()) {
            return int3(-1, -1, -1);
        }

        return rng.getRandomFromVector(validCandidates);
    }

    int maxMinDistSqRelative = -1;
    int maxMinDistSqTotal    = -1;
    vector<pair<int3, int>> tileDistances;
    tileDistances.reserve(freeTiles.size());

    for (const auto &pos : freeTiles) {
        int minDistToAnyObjectSqRelative = numeric_limits<int>::max();
        int minDistToAnyObjectSqTotal    = numeric_limits<int>::max();

        for (const auto &placedPos : placedSameObjects) {
            int dx     = pos.x - placedPos.x;
            int dy     = pos.y - placedPos.y;
            int distSq = dx * dx + dy * dy;
            if (distSq < minDistToAnyObjectSqRelative) {
                minDistToAnyObjectSqRelative = distSq;
            }
        }

        for (const auto &placedPos : placedAllObjects) {
            int dx     = pos.x - placedPos.x;
            int dy     = pos.y - placedPos.y;
            int distSq = dx * dx + dy * dy;
            if (distSq < minDistToAnyObjectSqTotal) {
                minDistToAnyObjectSqTotal = distSq;
            }
        }

        if (minDistanceTotal > 0 &&
            minDistToAnyObjectSqTotal < minDistanceTotal * minDistanceTotal) {
            continue;
        }

        tileDistances.push_back({pos, minDistToAnyObjectSqRelative});

        if (minDistToAnyObjectSqTotal > maxMinDistSqTotal) {
            maxMinDistSqTotal = minDistToAnyObjectSqTotal;
        }

        if (minDistToAnyObjectSqRelative > maxMinDistSqRelative) {
            maxMinDistSqRelative = minDistToAnyObjectSqRelative;
        }
    }

    if (minDistanceRelative > 0) {
        if (maxMinDistSqRelative < minDistanceRelative * minDistanceRelative) {
            return int3(-1, -1, -1);
        }
    }

    if (minDistanceTotal > 0) {
        if (maxMinDistSqTotal < minDistanceTotal * minDistanceTotal) {
            return int3(-1, -1, -1);
        }
    }
    int thresholdSq = static_cast<int>(maxMinDistSqRelative * (tolerance * tolerance));

    vector<int3> validCandidates;
    for (const auto &item : tileDistances) {
        if (item.second >= thresholdSq) {
            validCandidates.push_back(item.first);
        }
    }

    return rng.getRandomFromVector(validCandidates);
}

// int3 Map::findBestDistributedPosition(const vector<int3> &freeTiles,
//                                       const vector<int3> &placedObjects, const int3 &zoneCenter,
//                                       RNG &rng, float tolerance, int minDistance) {
//     if (freeTiles.empty())
//         return int3(-1, -1, -1);

//     if (placedObjects.empty()) {
//         int minCenterDistSq = numeric_limits<int>::max();

//         vector<pair<int3, int>> tileDistances;
//         tileDistances.reserve(freeTiles.size());

//         // TODO - OPTIMIZE
//         for (const auto &pos : freeTiles) {
//             int dx     = pos.x - zoneCenter.x;
//             int dy     = pos.y - zoneCenter.y;
//             int distSq = dx * dx + dy * dy;

//             tileDistances.push_back({pos, distSq});

//             if (distSq < minCenterDistSq) {
//                 minCenterDistSq = distSq;
//             }
//         }

//         int centerThresholdSq = minCenterDistSq + 8;

//         vector<int3> validCandidates;
//         for (const auto &item : tileDistances) {
//             if (item.second <= centerThresholdSq) {
//                 validCandidates.push_back(item.first);
//             }
//         }

//         return rng.getRandomFromVector(validCandidates);
//     }

//     int maxMinDistSq = -1;
//     vector<pair<int3, int>> tileDistances;
//     tileDistances.reserve(freeTiles.size());

//     for (const auto &pos : freeTiles) {
//         int minDistToAnyObjectSq = numeric_limits<int>::max();

//         for (const auto &placedPos : placedObjects) {
//             int dx     = pos.x - placedPos.x;
//             int dy     = pos.y - placedPos.y;
//             int distSq = dx * dx + dy * dy;
//             if (distSq < minDistToAnyObjectSq) {
//                 minDistToAnyObjectSq = distSq;
//             }
//         }

//         tileDistances.push_back({pos, minDistToAnyObjectSq});

//         if (minDistToAnyObjectSq > maxMinDistSq) {
//             maxMinDistSq = minDistToAnyObjectSq;
//         }
//     }

//     if (minDistance > 0) {
//         if (maxMinDistSq < minDistance * minDistance) {
//             cerr << "Failed to find a position with minDistance " << minDistance
//                  << " (maxMinDistSq was " << maxMinDistSq << ")\n";
//             return int3(-1, -1, -1);
//         }
//     }
//     int thresholdSq = static_cast<int>(maxMinDistSq * (tolerance * tolerance));

//     vector<int3> validCandidates;
//     for (const auto &item : tileDistances) {
//         if (item.second >= thresholdSq) {
//             validCandidates.push_back(item.first);
//         }
//     }

//     return rng.getRandomFromVector(validCandidates);
// }

void Map::printMap(int debugLevel) {

    if (debugLevel > 0) {
        // 1. ZARZĄDZANIE NAZWĄ PLIKU (zmieniłem domyślną nazwę, żeby odróżnić od zone grid)
        string baseFileName = "ch5-map";

        // Opcja B: Przez zmienną środowiskową
        if (const char *envName = std::getenv("GRID_OUT")) {
            baseFileName = envName;
        }

        string htmlFile = baseFileName + ".html";
        string pngFile  = baseFileName + ".png";

        // 2. KONFIGURACJA WIZUALNA
        bool useSpaces = false;
        ofstream out(htmlFile);

        out << "<pre style='background: #FFFFFF; color: #383A42; font-family: monospace; "
               "font-size: 26px; font-weight: 600; line-height: 1.2; padding: 40px; display: "
               "inline-block;'>\n";

        // 3. GENEROWANIE
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                TileType tileType = tileMap[i][j]->getTileType();
                char c            = tileTypeToChar(tileType);

                string hexColor;
                string ansiColor;
                bool isBold = false;

                // Mapowanie kolorów ANSI na przyjazne do druku HEXy
                switch (tileType) {
                case TileType::TILE_DEBUG:
                case TileType::TILE_OBSTACLE_BODY:
                    ansiColor = RED;
                    hexColor  = "#E45649"; // Czerwony
                    break;
                case TileType::TILE_FREE:
                    ansiColor = GREEN;
                    hexColor  = "#50A14F"; // Zielony
                    break;
                case TileType::TILE_OCCUPIED:
                    ansiColor = YELLOW;
                    hexColor  = "#C18401"; // Miodowy/Pomarańczowy (żółty byłby niewidoczny)
                    break;
                case TileType::TILE_ROAD:
                    ansiColor = MAGENTA;
                    hexColor  = "#A626A4"; // Magenta
                    break;
                case TileType::TILE_BORDER_INNER:
                    ansiColor = BLUE;
                    hexColor  = "#4078F2"; // Niebieski
                    break;
                case TileType::TILE_BORDER_OUTER:
                    ansiColor = BRIGHT_BLUE;
                    hexColor  = "#0953D6"; // Ciemniejszy, mocny niebieski dla kontrastu
                    break;
                case TileType::TILE_GUARD:
                    ansiColor = RED + BOLD;
                    hexColor  = "#E45649"; // Czerwony (pogrubiony flagą isBold)
                    isBold    = true;
                    break;
                case TileType::TILE_TAKEN:
                    ansiColor = CYAN;
                    hexColor  = "#FF8C00"; // Ciemny turkus, odróżnia się od niebieskich borderów
                    break;

                case TileType::TILE_RESERVED:
                    ansiColor = BRIGHT_BLUE;
                    hexColor  = "#DAA520"; // Neutralny szary, widoczny, ale mniej dominujący
                    break;

                default:
                    ansiColor = "";        // Domyślny kolor terminala
                    hexColor  = "#383A42"; // Domyślny ciemny szary tekstu
                    break;
                }

                // WYPISANIE W TERMINALU
                if (ansiColor.empty()) {
                    cerr << c;
                } else {
                    printColor(ansiColor, c);
                }

                // WYPISANIE DO PLIKU HTML
                if (isBold) {
                    out << "<span style='color: " << hexColor << "; font-weight: 900;'>" << c
                        << "</span>";
                } else {
                    out << "<span style='color: " << hexColor << ";'>" << c << "</span>";
                }

                // Dodajemy spację pomiędzy znakami (jeśli flaga useSpaces jest włączona)
                if (useSpaces && j < width - 1) {
                    out << " ";
                    cerr << " ";
                }
            }
            out << "\n";
            cerr << "\n";
        }

        // ZAMKNIĘCIE TAGU I PLIKU
        out << "</pre>\n";
        out.close();

        // SCREENSHOT
        string cmd = "google-chrome-stable --headless=new --hide-scrollbars "
                     "--window-size=4000,4000 --screenshot=" +
                     pngFile + " " + htmlFile + " > /dev/null 2>&1 && mogrify -trim " + pngFile;
        system(cmd.c_str());
    }

    // if (debugLevel > 2) {
    //     cerr << "==== Objects ====\n";
    //     for (auto &object : objectVector) {
    //         object->printObject();
    //     }

    //     cerr << "==== Monoliths ====\n";
    //     for (auto &monoliths : monolithVector) {
    //         cerr << "Monolith from ";
    //         monoliths.first->printObject();
    //         cerr << "Monolith dest ";
    //         monoliths.second->printObject();
    //     }
    // }
    // for (int i = 0; i < height; i++) {
    //     for (int j = 0; j < width; j++) {
    //         TileType tileType = tileMap[i][j]->getTileType();
    //         switch (tileType) {
    //         case TileType::TILE_DEBUG:
    //             printColor(RED, tileTypeToChar(tileType));
    //             break;
    //         case TileType::TILE_FREE:
    //             printColor(GREEN, tileTypeToChar(tileType));
    //             break;
    //         case TileType::TILE_OCCUPIED:
    //             printColor(YELLOW, tileTypeToChar(tileType));
    //             break;
    //         case TileType::TILE_RESERVED:
    //             cerr << tileTypeToChar(tileType);
    //             break;
    //         case TileType::TILE_TAKEN:
    //             printColor(CYAN, tileTypeToChar(tileType));
    //             break;
    //         case TileType::TILE_ROAD:
    //             printColor(MAGENTA, tileTypeToChar(tileType));
    //             break;
    //         case TileType::TILE_BORDER_INNER:
    //             printColor(BLUE, tileTypeToChar(tileType));
    //             break;
    //         case TileType::TILE_BORDER_OUTER:
    //             printColor(BRIGHT_BLUE, tileTypeToChar(tileType));
    //             break;
    //         case TileType::TILE_GUARD:
    //             printColor(RED + BOLD, tileTypeToChar(tileType));
    //             break;
    //         case TileType::TILE_OBSTACLE_BODY:
    //             printColor(RED, tileTypeToChar(tileType));
    //             break;
    //         default:
    //             cerr << tileTypeToChar(tileType);
    //         }
    //     }
    //     cerr << "\n";
    // }
}

void Map::printRealSizeMap() {
    cerr << "==== Map after placeObstacles (with realSize shown as 'X') ====\n";
    vector<string> tempMap(height, string(width, '.'));

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            tempMap[i][j] = tileTypeToChar(tileMap[i][j]->getTileType());
        }
    }

    for (auto &object : objectVector) {
        int3 pos                       = object->getPosition();
        int3 size                      = object->getSize();
        const vector<string> &realSize = object->getRealSize();

        if (realSize.empty() || (int)realSize.size() != size.y) {
            continue;
        }

        for (int dy = 0; dy < size.y; dy++) {
            if ((int)realSize[dy].size() != size.x)
                continue;

            for (int dx = 0; dx < size.x; dx++) {
                if (realSize[dy][dx] == '1') {
                    int tileX = pos.x - size.x + dx + 1;
                    int tileY = pos.y - size.y + dy + 1;

                    if (tileX >= 0 && tileX < width && tileY >= 0 && tileY < height) {
                        tempMap[tileY][tileX] = 'X';
                    }
                }
            }
        }
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (tempMap[y][x] == 'X') {
                printColor(RED, 'X');
            } else {
                TileType tileType = tileMap[y][x]->getTileType();
                switch (tileType) {
                case TileType::TILE_FREE:
                    printColor(GREEN, tempMap[y][x]);
                    break;
                case TileType::TILE_OCCUPIED:
                    printColor(YELLOW, tempMap[y][x]);
                    break;
                case TileType::TILE_TAKEN:
                    printColor(CYAN, tempMap[y][x]);
                    break;
                case TileType::TILE_ROAD:
                    printColor(MAGENTA, tempMap[y][x]);
                    break;
                case TileType::TILE_BORDER_INNER:
                    printColor(BLUE, tempMap[y][x]);
                    break;
                case TileType::TILE_BORDER_OUTER:
                    printColor(BRIGHT_BLUE, tempMap[y][x]);
                    break;
                case TileType::TILE_GUARD:
                    printColor(RED, tempMap[y][x]);
                    break;
                case TileType::TILE_OBSTACLE_BODY:
                    printColor(WHITE, tempMap[y][x]);
                    break;
                default:
                    cerr << tempMap[y][x];
                }
            }
        }
        cerr << "\n";
    }
}
