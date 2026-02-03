#include "./RegionPlacer.hpp"

RegionPlacer::RegionPlacer(Map &map) : map(map) {}

void RegionPlacer::initRegions() {
    LayoutInfo layout = map.getLayoutInfo();

    for (auto regionInfo : layout.getRegionInfoList()) {
        Region region(regionInfo);
        for (auto zoneLayout : regionInfo.getZoneLayoutList()) {
            Zone zone(zoneLayout);
            auto zonePtr = make_shared<Zone>(zone);
            region.addZone(zonePtr);
            map.addZone(zonePtr);
        }

        map.addRegion(make_shared<Region>(region));
    }
}

int RegionPlacer::calculateTotalSize() {
    LayoutInfo layout = map.getLayoutInfo();

    int sumOfSizes = 0;
    for (auto regionInfo : layout.getRegionInfoList()) {
        Region region(regionInfo);
        for (auto zoneLayout : regionInfo.getZoneLayoutList()) {
            Zone zone(zoneLayout);
            sumOfSizes += zone.getSize();
        }
    }
    return sumOfSizes;
}

void RegionPlacer::claimAbstractTile(int zoneID, int3 zoneCenter) {
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

void RegionPlacer::initGrid() {
    grid.clear();
    grid = vector<vector<int>>(gridN, vector<int>(gridN, 0));
}

int RegionPlacer::getPercentageSize(int zoneSize, int totalSize) {
    return floor(zoneSize * ((float)(gridN * gridN) / (float)(totalSize)));
}

void RegionPlacer::generateAbstractGrid() {
    int totalSize = calculateTotalSize();

    auto zoneMap = map.getZoneMap();
    vector<int> zoneIDs;
    for (auto [id, zone] : zoneMap)
        zoneIDs.push_back(id);

    gridN = ceil(sqrt(totalSize));

    initGrid();

    size_t x = 0, y = 0; // start from 0,0

    // TODO: copy random edge from engineer

    grid[x][y] = zoneIDs[0];

    int percentageSize = getPercentageSize(zoneMap[zoneIDs[0]]->getSize(), totalSize);
    for (int i = 1; i < percentageSize; i++) {
        claimAbstractTile(zoneIDs[0], int3(x, y, 0));
    }

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
        for (int i = 1; i < percentageSize; i++) {
            claimAbstractTile(zoneID, int3(bestPos.x, bestPos.y, 0));
        }
    }
}

bool RegionPlacer::validateAbstractGrid() {
    // TODO: check connectivity based on distancedBetweenZones
    return true;
}

void RegionPlacer::placeAbstractGridOnRealMap() {
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

void RegionPlacer::generateRegions() {
    int numberOfIterations = 100;
    while (numberOfIterations--) {
        generateAbstractGrid();
        if (validateAbstractGrid()) {
            break;
        }
    }
    if (numberOfIterations <= 0) {
        throw runtime_error("Could not generate valid abstract grid for regions!");
    }
    placeAbstractGridOnRealMap();
}

void RegionPlacer::calculateDistances() {
    LayoutInfo layout = map.getLayoutInfo();
    auto connections  = layout.getConnectionInfoList();

    auto zoneNeighbors = [&](const int zoneID) {
        vector<int> out;
        for (auto connection : connections) {
            if (connection.getZoneFrom() == zoneID) {
                out.push_back(connection.getZoneDest());
            }
            if (connection.getZoneDest() == zoneID) {
                out.push_back(connection.getZoneFrom());
            }
        }
        return out;
    };

    for (auto regionInfo : layout.getRegionInfoList()) {
        Region region(regionInfo);
        for (auto zoneLayout : regionInfo.getZoneLayoutList()) {
            Zone zone(zoneLayout);

            int zoneID                    = zone.getZoneID();
            distancedBetweenZones[zoneID] = bfs_distances<int>(zoneID, zoneNeighbors);
        }
    }
}

void RegionPlacer::claimTiles(vector<pair<int, int3>> &zoneTiles) {
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

void RegionPlacer::claimFreeTiles() {
    RegionMap regionMap = map.getRegionMap();

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

void RegionPlacer::calculateZoneCenters() {
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

void RegionPlacer::placeRegions() {
    initRegions();
    calculateDistances();

    generateRegions();

    claimFreeTiles();

    calculateZoneCenters();

    if (map.getLayoutInfo().getDebug() > 0) {

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
