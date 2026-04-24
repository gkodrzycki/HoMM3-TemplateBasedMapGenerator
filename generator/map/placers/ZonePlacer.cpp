#include "./ZonePlacer.hpp"

ZonePlacer::ZonePlacer(Map &map) : map(map) {}

void ZonePlacer::initZones() {
    TemplateInfo templateInfo = map.getTemplateInfo();

    for (auto zoneTemplate : templateInfo.getZones()) {
        Zone zone(zoneTemplate);
        auto zonePtr = make_shared<Zone>(zone);
        map.addZone(zonePtr);
    }
}

int ZonePlacer::calculateTotalSize() {
    // LayoutInfo layout = map.getLayoutInfo();

    int sumOfSizes = 0;

    auto zoneMap = map.getZoneMap();
    for (auto [id, zone] : zoneMap) {
        cerr << "Zone " << id << " has size " << zone->getSize() << "\n";
        sumOfSizes += zone->getSize();
    }

    // for (auto regionInfo : layout.getRegionInfoList()) {
    //     Region region(regionInfo);
    //     for (auto zoneLayout : regionInfo.getZoneLayoutList()) {
    //         Zone zone(zoneLayout);
    //         sumOfSizes += zone.getSize();
    //     }
    // }
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
        std::array<int3, 4> out;
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

    // TODO: better selection based on distance to zoneCenter

    for (const auto &tile : filteredTiles) {
        grid[tile.x][tile.y] = zoneID;
        break;
    }
}

void ZonePlacer::initGrid() {
    grid.clear();
    grid = vector<vector<int>>(gridN, vector<int>(gridN, 0));
}

int ZonePlacer::getPercentageSize(int zoneSize, int totalSize) {
    int tempGridN = gridN - 2;
    return floor(zoneSize * ((float)(tempGridN * tempGridN) / (float)(totalSize)));
}

void ZonePlacer::generateAbstractGrid() {
    int totalSize = calculateTotalSize();

    auto zoneMap = map.getZoneMap();
    vector<int> zoneIDs;
    for (auto [id, zone] : zoneMap)
        zoneIDs.push_back(id);

    gridN = ceil(sqrt(totalSize)) + 2;

    initGrid();

    size_t x = 0, y = 0; // start from 0,0
    if (zoneMap[zoneIDs[0]]->getType() == "human_start") {
        x = 0;
        y = 0;
    } else {
        RNG &rng = map.getRNG();
        x        = gridN / 2;
        y        = x;
    }
    // TODO: copy random edge from engineer

    grid[x][y] = zoneIDs[0];

    int percentageSize = getPercentageSize(zoneMap[zoneIDs[0]]->getSize(), totalSize);
    // for (int i = 1; i < percentageSize; i++) {
    //     claimAbstractTile(zoneIDs[0], int3(x, y, 0));
    // }

    for (size_t i = 1; i < zoneIDs.size(); i++) {
        int zoneID = zoneIDs[i];

        float bestDist = numeric_limits<float>::lowest();
        int3 bestPos;

        for (int freeX = 0; freeX < gridN; freeX++) {
            for (int freeY = 0; freeY < gridN; freeY++) {
                int3 freePos = int3(freeX, freeY, 0);
                if (grid[freeX][freeY] != 0)
                    continue;

                float scoreOfPos = 0;
                for (int otherX = 0; otherX < gridN; otherX++) {
                    for (int otherY = 0; otherY < gridN; otherY++) {
                        int3 otherPos   = int3(otherX, otherY, 0);
                        int otherZoneID = grid[otherX][otherY];
                        if (otherZoneID == 0)
                            continue;

                        float abstractDist = distancedBetweenZones[zoneID][otherZoneID];
                        if (abstractDist == 1) {
                            scoreOfPos -= freePos.distance2DMH(otherPos);
                        } else {
                            scoreOfPos += freePos.distance2DMH(otherPos) * (abstractDist + 1);
                        }
                    }
                }
                if (scoreOfPos > bestDist) {
                    bestDist = scoreOfPos;
                    bestPos  = freePos;
                }
            }
        }
        grid[bestPos.x][bestPos.y] = zoneID;

        int percentageSize = getPercentageSize(zoneMap[zoneID]->getSize(), totalSize);
        // for (int i = 1; i < percentageSize; i++) {
        //     claimAbstractTile(zoneID, int3(bestPos.x, bestPos.y, 0));
        // }
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
        for (auto [otherZoneID, dist] : distancedBetweenZones[zoneID]) {
            cerr << "Distance between zone " << zoneID << " and " << otherZoneID << " is " << dist
                 << "\n";
        }
    }

    // for (auto regionInfo : layout.getRegionInfoList()) {
    //     Region region(regionInfo);
    //     for (auto zoneLayout : regionInfo.getZoneLayoutList()) {
    //         Zone zone(zoneLayout);

    //         int zoneID                    = zone.getZoneID();
    //         distancedBetweenZones[zoneID] = bfs_distances<int>(zoneID, zoneNeighbors);
    //     }
    // }
}

void ZonePlacer::claimTiles(vector<pair<int, int3>> &zoneTiles) {
    int mapWidth = map.getWidth(), mapHeight = map.getHeight();

    auto neighbors4 = [&](const int3 &p) {
        std::array<int3, 4> out;
        for (int i = 0; i < 4; i++)
            out[i] = p + directions4[i];
        return out;
    };

    auto passable = [&](const int3 &p) { return true; };

    auto claim = bfs_claim_xyz2(mapWidth, mapHeight, zoneTiles, neighbors4, passable);

    ZoneMap zoneMap = map.getZoneMap();

    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            int zoneID   = claim[x][y][0];
            auto tilePtr = map.getTile(int3(x, y, 0));
            tilePtr->setZoneID(zoneID);
            tilePtr->setTerrain(zoneMap[zoneID]->getTerrain());
        }
    }
}

void ZonePlacer::claimFreeTiles() {
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
    claimTiles(zoneTiles);
}

void ZonePlacer::calculateZoneCenters() {
    ZoneMap zoneMap = map.getZoneMap();

    std::map<int, vector<int3>> zoneTiles;

    for (auto &[zoneID, zone] : zoneMap) {
        for (int y = 0; y < map.getHeight(); y++) {
            for (int x = 0; x < map.getWidth(); x++) {
                auto tilePtr = map.getTile(int3(x, y, 0));
                if (tilePtr->getZoneID() != zoneID)
                    continue;
                zoneTiles[zoneID].push_back(int3(x, y, 0));
            }
        }
    }

    for (auto &[zoneID, zone] : zoneMap) {
        // take tile with lowest distance to others within same zone
        int3 bestPos;
        float bestDist = numeric_limits<float>::max();

        for (size_t i = 0; i < zoneTiles[zoneID].size(); i++) {
            float distSum = 0;
            for (const auto &pos : zoneTiles[zoneID]) {
                distSum += pos.distance2DMH(zoneTiles[zoneID][i]);
            }
            if (distSum < bestDist) {
                bestDist = distSum;
                bestPos  = zoneTiles[zoneID][i];
            }
        }
        zone->setCenter(bestPos);
    }
}

void ZonePlacer::placeZones() {
    initZones();
    calculateDistances();
    generateZones();
    claimFreeTiles();

    calculateZoneCenters();

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
