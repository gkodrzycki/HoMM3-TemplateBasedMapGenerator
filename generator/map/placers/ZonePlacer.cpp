#include "./ZonePlacer.hpp"

ZonePlacer::ZonePlacer(Map &map) : map(map) {}

void ZonePlacer::initZones() {
    TemplateInfo templateInfo = map.getTemplateInfo();

    for (auto zoneTemplate : templateInfo.getZones()) {
        Zone zone(zoneTemplate, map.getRNG());
        auto zonePtr = make_shared<Zone>(zone);
        map.addZone(zonePtr);
    }
}

string ZonePlacer::generateZoneHash(int zoneID) {
    string zoneHash = "";

    TemplateInfo templateInfo = map.getTemplateInfo();
    auto zoneTemplate         = templateInfo.getZoneById(zoneID);

    zoneHash += zoneTemplate.getType() + "_";

    for (auto [mineType, mineCount] : zoneTemplate.getMinimumMines().mineCounts) {
        string enumName = getEnumName(mineType);
        zoneHash += enumName + "_" + to_string(mineCount) + "_";
    }

    auto connectionVector = templateInfo.getConnections();
    for (auto connection : connectionVector) {
        if (connection.getZone1() == zoneID) {
            zoneHash += to_string(connection.getZone2()) + "_";
        } else if (connection.getZone2() == zoneID) {
            zoneHash += to_string(connection.getZone1()) + "_";
        }
    }
    zoneHash += "#";

    return zoneHash;
}

void ZonePlacer::groupZones() {

    GroupSettingMap &groupSettingMap = map.getGroupSettingMap();
    auto zoneMap                     = map.getZoneMap();
    for (auto [zoneID, zone] : zoneMap) {
        string zoneHash = generateZoneHash(zoneID);
        map.addHash(zoneID, zoneHash);
        groupSettingMap[zoneHash] = make_shared<GroupSetting>(GroupSetting());
    }
}

int ZonePlacer::calculateTotalSize() {
    int sumOfSizes = 0;

    auto zoneMap = map.getZoneMap();
    for (auto [id, zone] : zoneMap) {
        sumOfSizes += zone->getSize();
    }
    return sumOfSizes;
}

void ZonePlacer::claimAbstractTile(int zoneID, int3 zoneCenter) {
    vector<int3> zoneTiles;
    for (int x = 0; x < gridN; x++) {
        for (int y = 0; y < gridN; y++) {
            if (grid[x][y] == zoneID) {
                zoneTiles.push_back(int3(x, y, 0));
            }
        }
    }

    auto neighbors4 = [&](const int3 &p) {
        array<int3, 4> out;
        for (int i = 0; i < 4; i++)
            out[i] = p + directions4[i];
        return out;
    };

    auto passable = [&](const int3 &p) { return true; };

    auto possibleTiles = bfs_collect_depth_xy(gridN, gridN, zoneTiles, 1, neighbors4, passable);

    vector<int3> filteredTiles;
    for (const auto &tile : possibleTiles) {
        if (grid[tile.x][tile.y] == 0) {
            filteredTiles.push_back(tile);
        }
    }
    RNG &rng = map.getRNG();
    rng.shuffle(filteredTiles);

    if (filteredTiles.empty()) {
        return;
    }

    int3 bestTile   = filteredTiles[0];
    float bestScore = bestTile.distance2DSQ(zoneCenter) + rng.nextFloat(0.0, 2.5f);

    for (const auto &tile : filteredTiles) {
        float dist = tile.distance2DSQ(zoneCenter);

        // TODO: Calibrate parameter for noise
        float randomNoise = rng.nextFloat(0.0, 1.5f);

        float score = dist + randomNoise;

        if (score < bestScore) {
            bestScore = score;
            bestTile  = tile;
        }
    }
    grid[bestTile.x][bestTile.y] = zoneID;
}

void ZonePlacer::initGrid() {
    grid.clear();
    grid = vector<vector<int>>(gridN, vector<int>(gridN, 0));
}

int ZonePlacer::getPercentageSize(int zoneSize, int gridN, int totalSize) {
    float percentage = floor(zoneSize * ((float)(gridN * gridN) / (float)(totalSize)));
    if (percentage < 1) {
        percentage = 1;
    }
    return (int)percentage;
}

void ZonePlacer::generateAbstractGrid() {
    int totalSize = calculateTotalSize();

    auto zoneMap = map.getZoneMap();
    vector<int> zoneIDs;
    for (auto [id, zone] : zoneMap)
        zoneIDs.push_back(id);

    gridN = ceil(sqrt(totalSize)) + 3;

    initGrid();

    RNG &rng = map.getRNG();

    struct ZonePos {
        int id;
        int3 pos;
    };
    vector<ZonePos> centers;

    for (int zoneID : zoneIDs) {
        while (true) {
            int rx = rng.nextInt(0, gridN - 1);
            int ry = rng.nextInt(0, gridN - 1);
            if (grid[rx][ry] == 0) {
                grid[rx][ry] = zoneID;
                centers.push_back({zoneID, int3(rx, ry, 0)});
                break;
            }
        }
    }

    auto calculateEnergy = [&]() {
        float energy = 0.0f;
        for (size_t i = 0; i < centers.size(); i++) {
            for (size_t j = i + 1; j < centers.size(); j++) {
                if (i == j)
                    continue;
                int z1             = centers[i].id;
                int z2             = centers[j].id;
                float mapDist      = centers[i].pos.distance2DMH(centers[j].pos);
                float abstractDist = distancedBetweenZones[z1][z2];

                if (abstractDist == 1) {
                    energy += mapDist * mapDist;
                } else {
                    if (mapDist > 0) {
                        energy += (500.0f * abstractDist) / mapDist;
                    }
                }
            }
        }
        return energy;
    };

    float temperature    = 100.0f;
    float coolingRate    = 0.999f;
    float minTemperature = 0.01f;
    float currentEnergy  = calculateEnergy();

    while (temperature > minTemperature) {
        int randomIdx = rng.nextInt(0, centers.size() - 1);
        int3 oldPos   = centers[randomIdx].pos;

        int rx = rng.nextInt(0, gridN - 1);
        int ry = rng.nextInt(0, gridN - 1);
        int3 newPos(rx, ry, 0);

        if (oldPos == newPos)
            continue;

        int targetZoneID = grid[rx][ry];

        grid[oldPos.x][oldPos.y] = targetZoneID;
        grid[newPos.x][newPos.y] = centers[randomIdx].id;
        centers[randomIdx].pos   = newPos;

        int targetZoneIdx = -1;
        if (targetZoneID != 0) {
            for (size_t i = 0; i < centers.size(); i++) {
                if (centers[i].id == targetZoneID) {
                    targetZoneIdx              = i;
                    centers[targetZoneIdx].pos = oldPos;
                    break;
                }
            }
        }

        float newEnergy   = calculateEnergy();
        float deltaEnergy = newEnergy - currentEnergy;
        bool accept       = false;

        if (deltaEnergy < 0) {
            accept = true;
        } else {
            float randProb = rng.nextFloat(0.0f, 1.0f);
            if (randProb < exp(-deltaEnergy / temperature)) {
                accept = true;
            }
        }

        if (accept) {
            currentEnergy = newEnergy;
        } else {
            grid[oldPos.x][oldPos.y] = centers[randomIdx].id;
            grid[newPos.x][newPos.y] = targetZoneID;
            centers[randomIdx].pos   = oldPos;
            if (targetZoneIdx != -1) {
                centers[targetZoneIdx].pos = newPos;
            }
        }

        temperature *= coolingRate;
    }

    for (const auto &center : centers) {
        int zoneID         = center.id;
        int percentageSize = getPercentageSize(zoneMap[zoneID]->getSize(), gridN - 2, totalSize);
        for (int i = 1; i < percentageSize; i++) {
            claimAbstractTile(zoneID, center.pos);
        }
    }
}

bool ZonePlacer::validateAbstractGrid() {
    // TODO: check connectivity based on distancedBetweenZones
    return true;
}

void ZonePlacer::placeAbstractGridOnRealMap() {
    auto zoneMap      = map.getZoneMap();
    int realMapSize   = map.getWidth();
    int numberOfTiles = realMapSize / gridN;

    for (int i = 0; i < gridN; i++) {
        for (int j = 0; j < gridN; j++) {
            auto zoneID = grid[i][j];
            if (zoneID == 0)
                continue;
            for (int tileX = numberOfTiles * j; tileX <= numberOfTiles * (j + 1); tileX++) {
                for (int tileY = numberOfTiles * i; tileY <= numberOfTiles * (i + 1); tileY++) {
                    auto tilePtr = map.getTile(int3(tileX, tileY, 0));
                    if (tilePtr == nullptr)
                        continue;
                    tilePtr->setZoneID(zoneID);
                    tilePtr->setTerrain(zoneMap[zoneID]->getTerrain());
                }
            }
        }
    }
}

void ZonePlacer::generateZones() {
    int numberOfIterations = 100;
    while (numberOfIterations--) {
        generateAbstractGrid();
        if (validateAbstractGrid()) {
            break;
        }
    }
    if (numberOfIterations <= 0) {
        throw runtime_error("Could not generate valid abstract grid for regions using seed " +
                            to_string(map.getRNG().getOriginalSeed()));
    }
    placeAbstractGridOnRealMap();
}

void ZonePlacer::calculateDistances() {
    TemplateInfo templateInfo = map.getTemplateInfo();
    auto connections          = templateInfo.getConnections();

    auto zoneNeighbors = [&](const int zoneID) {
        vector<int> out;
        for (auto connection : connections) {
            if (connection.getZone1() == zoneID) {
                out.push_back(connection.getZone2());
            }
            if (connection.getZone2() == zoneID) {
                out.push_back(connection.getZone1());
            }
        }
        return out;
    };

    auto zoneMap = map.getZoneMap();
    for (auto [zoneID, zone] : zoneMap) {
        distancedBetweenZones[zoneID] = bfs_distances<int>(zoneID, zoneNeighbors);
    }
}

bool ZonePlacer::claimTiles(vector<pair<int, int3>> &zoneTiles, bool fullClaim) {
    int mapWidth = map.getWidth(), mapHeight = map.getHeight();

    auto neighbors4 = [&](const int3 &p) {
        array<int3, 4> out;
        for (int i = 0; i < 4; i++)
            out[i] = p + directions4[i];
        return out;
    };

    std::map<int, int> currentZoneSize;
    std::map<int, int> maxZoneSize;
    for (const auto &[zoneID, zone] : map.getZoneMap()) {
        maxZoneSize[zoneID] =
            getPercentageSize(zone->getSize(), map.getWidth(), calculateTotalSize());
    }

    auto passable = [&](const int3 &p) { return true; };

    auto claimFn = [&](int zoneID, const int3 &pos) {
        if (fullClaim)
            return true;
        currentZoneSize[zoneID]++;
        if (currentZoneSize[zoneID] > maxZoneSize[zoneID] * 1.1)
            return false;
        return true;
    };

    auto claim = bfs_claim_xyz2(mapWidth, mapHeight, zoneTiles, neighbors4, passable, claimFn);

    ZoneMap zoneMap = map.getZoneMap();

    bool fullyClaimed = true;
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            int zoneID = claim[x][y][0];
            if (!fullClaim && zoneID == 0) {
                fullyClaimed = false;
                continue;
            } else if (zoneID == 0) {
                throw runtime_error("Tile at (" + to_string(x) + ", " + to_string(y) +
                                    ") was not claimed by any zone during claiming process.");
            }
            auto tilePtr = map.getTile(int3(x, y, 0));
            tilePtr->setZoneID(zoneID);
            tilePtr->setTerrain(zoneMap[zoneID]->getTerrain());
        }
    }
    return fullyClaimed;
}

bool ZonePlacer::claimFreeTiles(bool fullClaim) {
    vector<pair<int, int3>> zoneTiles;

    for (int y = 0; y < map.getHeight(); y++) {
        for (int x = 0; x < map.getWidth(); x++) {
            auto tilePtr = map.getTile(int3(x, y, 0));
            int zoneID   = tilePtr->getZoneID();
            if (zoneID == 0)
                continue;
            zoneTiles.push_back({zoneID, int3(x, y, 0)});
        }
    }
    return claimTiles(zoneTiles, fullClaim);
}

void ZonePlacer::calculateZoneCenters() {
    ZoneMap zoneMap = map.getZoneMap();
    int mapWidth    = map.getWidth();
    int mapHeight   = map.getHeight();

    std::map<int, vector<int3>> zoneTiles;

    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            auto tilePtr = map.getTile(int3(x, y, 0));
            int zoneID   = tilePtr->getZoneID();
            if (zoneID != 0) {
                zoneTiles[zoneID].push_back(int3(x, y, 0));
            }
        }
    }

    auto neighbors4 = [&](const int3 &p) {
        array<int3, 4> out;
        for (int i = 0; i < 4; i++)
            out[i] = p + directions4[i];
        return out;
    };

    for (auto &[zoneID, zone] : zoneMap) {
        if (zoneTiles[zoneID].empty())
            continue;

        vector<int3> borderTiles;

        for (const auto &pos : zoneTiles[zoneID]) {
            bool isBorder = false;
            for (const auto &nPos : neighbors4(pos)) {
                if (nPos.x < 0 || nPos.x >= mapWidth || nPos.y < 0 || nPos.y >= mapHeight ||
                    map.getTile(nPos)->getZoneID() != zoneID) {
                    isBorder = true;
                    break;
                }
            }

            if (isBorder) {
                borderTiles.push_back(pos);
            }
        }

        if (borderTiles.empty()) {
            zone->setCenter(zoneTiles[zoneID].front());
            continue;
        }

        auto visitedNodes = bfs_collect_depth_xy(
            mapWidth, mapHeight, borderTiles, numeric_limits<int>::max(), neighbors4,
            [&](const int3 &p) { return map.getTile(p)->getZoneID() == zoneID; });

        // last element in visitedNodes will be the farthest from border, so it will be in the
        // center
        zone->setCenter(visitedNodes.back());
    }
}
void ZonePlacer::roughenBoundaries() {
    auto &rng = map.getRNG();

    FastNoiseLite noiseX, noiseY;
    noiseX.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noiseX.SetFrequency(0.04f);
    noiseX.SetSeed(rng.getSeed());

    noiseY.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noiseY.SetFrequency(0.04f);
    noiseY.SetSeed(rng.getSeed() + 1000);

    int mapWidth = map.getWidth(), mapHeight = map.getHeight();
    float strength = 6.0f;

    vector<vector<int>> snapshot(mapWidth, vector<int>(mapHeight, 0));
    for (int y = 0; y < mapHeight; y++)
        for (int x = 0; x < mapWidth; x++)
            snapshot[x][y] = map.getTile(int3(x, y, 0))->getZoneID();

    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            float dx = noiseX.GetNoise((float)x, (float)y) * strength;
            float dy = noiseY.GetNoise((float)x, (float)y) * strength;

            int sx = std::clamp((int)(x + dx), 0, mapWidth - 1);
            int sy = std::clamp((int)(y + dy), 0, mapHeight - 1);

            map.getTile(int3(x, y, 0))->setZoneID(snapshot[sx][sy]);
        }
    }
}

void ZonePlacer::relaxOutliers(float thresholdFactor) {
    ZoneMap zoneMap = map.getZoneMap();
    int mapWidth = map.getWidth(), mapHeight = map.getHeight();

    std::map<int, vector<float>> distancesPerZone;

    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            int zoneID = map.getTile(int3(x, y, 0))->getZoneID();
            if (zoneID == 0)
                continue;
            int3 center = zoneMap[zoneID]->getCenter();
            float d     = sqrt((float)int3(x, y, 0).distance2DSQ(center));
            distancesPerZone[zoneID].push_back(d);
        }
    }

    std::map<int, float> threshold;
    for (auto &[zoneID, dists] : distancesPerZone) {
        vector<float> sorted = dists;
        sort(sorted.begin(), sorted.end());
        float median      = sorted[sorted.size() / 2];
        threshold[zoneID] = median * thresholdFactor;
    }

    vector<vector<int>> newZoneID(mapWidth, vector<int>(mapHeight, 0));
    for (int y = 0; y < mapHeight; y++)
        for (int x = 0; x < mapWidth; x++)
            newZoneID[x][y] = map.getTile(int3(x, y, 0))->getZoneID();

    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            int3 pos(x, y, 0);
            int zoneID = map.getTile(pos)->getZoneID();
            if (zoneID == 0)
                continue;

            int3 ownCenter = zoneMap[zoneID]->getCenter();
            float ownDist  = sqrt((float)pos.distance2DSQ(ownCenter));

            if (ownDist <= threshold[zoneID])
                continue;

            int bestZone   = zoneID;
            float bestDist = ownDist;
            for (auto &[zid, zone] : zoneMap) {
                float d = sqrt((float)pos.distance2DSQ(zone->getCenter()));
                if (d < bestDist) {
                    bestDist = d;
                    bestZone = zid;
                }
            }
            newZoneID[x][y] = bestZone;
        }
    }

    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            int newZ     = newZoneID[x][y];
            auto tilePtr = map.getTile(int3(x, y, 0));
            if (newZ != tilePtr->getZoneID()) {
                tilePtr->setZoneID(newZ);
                tilePtr->setTerrain(zoneMap[newZ]->getTerrain());
            }
        }
    }
}

void ZonePlacer::fixDisjointZones() {
    ZoneMap zoneMap = map.getZoneMap();
    int mapWidth = map.getWidth(), mapHeight = map.getHeight();

    for (auto &[zoneID, zone] : zoneMap) {
        int3 center = zone->getCenter();

        auto neighbors4 = [&](const int3 &p) {
            array<int3, 4> out;
            for (int i = 0; i < 4; i++)
                out[i] = p + directions4[i];
            return out;
        };

        auto passable = [&](const int3 &p) { return map.getTile(p)->getZoneID() == zoneID; };

        vector<int3> sources = {center};
        auto connectedTiles  = bfs_collect_depth_xy(
            mapWidth, mapHeight, sources, numeric_limits<int>::max(), neighbors4, passable);

        std::map<int3, bool> isConnected;
        for (const auto &tile : connectedTiles) {
            isConnected[tile] = true;
        }

        for (int y = 0; y < mapHeight; y++) {
            for (int x = 0; x < mapWidth; x++) {
                int3 pos(x, y, 0);
                if (map.getTile(pos)->getZoneID() == zoneID && isConnected[pos] == false) {
                    map.getTile(pos)->setZoneID(0);
                }
            }
        }
    }

    claimFreeTiles(true);
}

void ZonePlacer::placeZones() {
    initZones();
    calculateDistances();
    generateZones();
    roughenBoundaries();
    bool allClaimed = claimFreeTiles();
    if (!allClaimed) {
        cerr << "Not all tiles were claimed in the first pass, claiming remaining tiles with "
                "fullClaim=true\n";
        claimFreeTiles(true);
    }

    std::map<int, int> zoneSizes;
    auto zoneMap = map.getZoneMap();
    for (auto [zoneID, zone] : zoneMap) {
        zoneSizes[zoneID] = 0;
    }
    for (int y = 0; y < map.getHeight(); y++) {
        for (int x = 0; x < map.getWidth(); x++) {
            auto tilePtr = map.getTile(int3(x, y, 0));
            int zoneID   = tilePtr->getZoneID();
            if (zoneID != 0) {
                zoneSizes[zoneID]++;
            }
        }
    }

    calculateZoneCenters();
    for (int i = 0; i < 4; i++) {
        relaxOutliers(1.8f - 0.1f * i);
        calculateZoneCenters();
    }

    fixDisjointZones();

    if (map.getTemplateInfo().getDebug() > 0) {
        cerr << "=== Abstract grid ===\n";
        for (int i = 0; i < gridN; i++) {
            for (int j = 0; j < gridN; j++) {
                printColor(getZoneColor(grid[i][j]), grid[i][j]);
            }
            cerr << "\n";
        }

        cerr << "=== Map zones ===\n";
        for (int i = 0; i < map.getHeight(); i++) {
            for (int j = 0; j < map.getWidth(); j++) {
                auto tilePtr = map.getTile(int3(j, i, 0));

                auto zoneMap = map.getZoneMap();
                auto zoneID  = tilePtr->getZoneID();
                if (zoneMap[zoneID]->getCenter() == int3(j, i, 0)) {
                    printColor(BOLD + RED, tilePtr->getZoneID());
                    continue;
                }
                printColor(getZoneColor(tilePtr->getZoneID()), tilePtr->getZoneID());
            }
            cerr << "\n";
        }
    }
}
