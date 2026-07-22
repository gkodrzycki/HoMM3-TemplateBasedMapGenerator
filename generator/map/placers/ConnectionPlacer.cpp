#include "./ConnectionPlacer.hpp"

ConnectionPlacer::ConnectionPlacer(Map &map) : map(map) {
    failedRoadConnections.resize(map.getTemplateInfo().getConnections().size(), false);
}

void ConnectionPlacer::placeRoads() {
    std::map<int, vector<pair<int, int3>>> connectionsPoints = getConnectionsPoints();
    ZoneMap zoneMap                                          = map.getZoneMap();

    for (auto [zoneID, zone] : zoneMap) {
        auto zoneConnectionPoints = connectionsPoints[zoneID];
        for (size_t i = 0; i < zoneConnectionPoints.size(); i++) {
            for (size_t j = i + 1; j < zoneConnectionPoints.size(); j++) {
                int3 fromPos = connectionsPoints[zoneID][i].second;
                int3 destPos = connectionsPoints[zoneID][j].second;

                vector<int3> path = createPath(fromPos, destPos);

                if (path.empty()) {
                    throw runtime_error("Failed to create a path between points in zone " +
                                        to_string(zoneID) + " on seed " +
                                        to_string(map.getRNG().getOriginalSeed()) + "\n");
                }

                Road road(2, path, "Road");
                auto roadPtr = make_shared<Road>(road);
                map.addRoad(roadPtr);

                for (const auto &pos : path) {
                    map.getTile(pos)->setTileType(TileType::TILE_ROAD);
                    int3 posBelow = pos + int3(0, 1, 0);

                    if (!map.getTile(posBelow)->isTileType("rTB"))
                        map.getTile(posBelow)->setTileType(TileType::TILE_RESERVED);
                }
            }
        }
    }

    auto connections = map.getTemplateInfo().getConnections();
    for (size_t connectionID = 0; connectionID < connections.size(); connectionID++) {
        const auto connection = connections[connectionID];
        if (connection.getType() == "teleport" || connection.getType() == "underground")
            continue;

        int3 fromPos = connectionsPoints[connection.getZone1()][0].second;
        int3 destPos = connectionsPoints[connection.getZone2()][0].second;

        vector<int3> path = createPath(fromPos, destPos);

        if (path.empty()) {
            cerr << "Failed to create a path between zones " << connection.getZone1() << " and "
                 << connection.getZone2() << " on seed " << map.getRNG().getOriginalSeed()
                 << " changing type to teleport\n";
            failedRoadConnections[connectionID] = true;
            continue;
        }

        Road road(2, path, "Road");
        auto roadPtr = make_shared<Road>(road);
        map.addRoad(roadPtr);

        for (const auto &pos : path) {
            map.getTile(pos)->setTileType(TileType::TILE_ROAD);
            int3 posBelow = pos + int3(0, 1, 0);

            if (!map.getTile(posBelow)->isTileType("rTB"))
                map.getTile(posBelow)->setTileType(TileType::TILE_RESERVED);
        }
    }
}

void ConnectionPlacer::createMonoliths() {
    int mapWidth                                             = map.getWidth();
    int mapHeight                                            = map.getHeight();
    auto &rng                                                = map.getRNG();
    std::map<int, vector<pair<int, int3>>> connectionsPoints = getConnectionsPoints();

    std::map<int, vector<int3>> zoneTiles;

    for (int x = 0; x < mapWidth; x++) {
        for (int y = 0; y < mapHeight; y++) {
            int3 tilePos = int3(x, y, 0);
            auto tile    = map.getTile(tilePos);
            int zoneID   = tile->getZoneID();
            if (tile->isTileType("FO")) {
                zoneTiles[zoneID].push_back(tilePos);
            }
        }
    }

    int connectionCount    = 0;
    int distanceFromCenter = 20; // Distance of monoliths from zone centers to avoid blocking them
    auto connections       = map.getTemplateInfo().getConnections();
    for (size_t connectionID = 0; connectionID < connections.size(); connectionID++) {
        const auto &connection = connections[connectionID];
        if (connection.getType() != "teleport" && !failedRoadConnections[connectionID]) {
            continue;
        }

        int3 centerFromPos = connectionsPoints[connection.getZone1()][0].second;
        int3 centerDestPos = connectionsPoints[connection.getZone2()][0].second;

        vector<int3> fromZoneTiles = zoneTiles[connection.getZone1()];
        vector<int3> destZoneTiles = zoneTiles[connection.getZone2()];

        // filter tiles which are exactly distanceFromCenter away from the center of their zone
        fromZoneTiles.erase(remove_if(fromZoneTiles.begin(), fromZoneTiles.end(),
                                      [&](int3 pos) {
                                          return pos.distance2DMH(centerFromPos) !=
                                                 distanceFromCenter;
                                      }),
                            fromZoneTiles.end());
        destZoneTiles.erase(remove_if(destZoneTiles.begin(), destZoneTiles.end(),
                                      [&](int3 pos) {
                                          return pos.distance2DMH(centerDestPos) !=
                                                 distanceFromCenter;
                                      }),
                            destZoneTiles.end());

        if (fromZoneTiles.empty() || destZoneTiles.empty()) {
            throw runtime_error("Failed to find suitable tiles for monoliths in zones " +
                                to_string(connection.getZone1()) + " and " +
                                to_string(connection.getZone2()) + " on seed " +
                                to_string(map.getRNG().getOriginalSeed()) + "\n");
        }

        int3 fromPos = rng.getRandomFromVector(fromZoneTiles);
        int3 destPos = rng.getRandomFromVector(destZoneTiles);
        int3 size = (connectionCount == 0 || connectionCount == 5) ? int3(1, 1, 1) : int3(2, 1, 1);
        vector<string> realSize = (connectionCount == 0 || connectionCount == 5)
                                      ? vector<string>{"1"}
                                      : vector<string>{"11"};

        Object monolithFrom(fromPos, "Monolith", size);
        Object monolithDest(destPos, "Monolith", size);

        auto monolithFromPtr = make_shared<Object>(monolithFrom);
        auto monolithDestPtr = make_shared<Object>(monolithDest);

        map.addMonoliths(monolithFromPtr, monolithDestPtr);

        map.fixNeighbourTiles(fromPos, size, realSize, map.getTile(fromPos)->getZoneID());
        map.fixNeighbourTiles(destPos, size, realSize, map.getTile(destPos)->getZoneID());
        connectionCount++;
    }
}

vector<int3> ConnectionPlacer::createPath(int3 fromPos, int3 destPos) {
    int mapWidth = map.getWidth(), mapHeight = map.getHeight();
    RNG &rng = map.getRNG();

    const int TRIES = 5;
    int forcedCount = 0;

    int fromZoneID = map.getTile(fromPos)->getZoneID();
    int destZoneID = map.getTile(destPos)->getZoneID();

    // Chiseled paths state: 0=OPEN, 1=BLOCKED, 2=FORCED
    vector<vector<int>> state(mapWidth, vector<int>(mapHeight, 0));
    for (int x = 0; x < mapWidth; x++) {
        for (int y = 0; y < mapHeight; y++) {
            int3 p(x, y, 0);
            if (x == 0 || x == mapWidth - 1 || y == 0 || y == mapHeight - 1) {
                state[x][y] = 1;
                continue;
            }
            auto tile = map.getTile(p);
            if (!tile || tile->isTileType("T")) {
                state[x][y] = 1;
                continue;
            }
            if (!tile || tile->isTileType("r")) {
                state[x][y] = 2;
                continue;
            }

            if (tile->getZoneID() != fromZoneID && tile->getZoneID() != destZoneID)
                state[x][y] = 1;
        }
    }

    state[fromPos.x][fromPos.y] = 2;
    state[destPos.x][destPos.y] = 2;
    forcedCount                 = 2;

    // Zone-monotone BFS: once the path crosses into destZoneID it may not return to fromZoneID.
    // Phase 0 = still in fromZone, Phase 1 = crossed into destZone (stays there).
    struct BFSState {
        int type, distance, x, y, phase;
        bool operator<(const BFSState &o) const {
            if (type != o.type)
                return type < o.type;     // prefer lower type (OPEN < BLOCKED < FORCED)
            return distance > o.distance; // prefer shorter distance
        }
    };

    vector<vector<array<bool, 2>>> bfsVis(mapWidth,
                                          vector<array<bool, 2>>(mapHeight, {false, false}));
    vector<vector<array<BFSState, 2>>> bfsPar(
        mapWidth, vector<array<BFSState, 2>>(mapHeight,
                                             {BFSState{-1, -1, -1, -1}, BFSState{-1, -1, -1, -1}}));

    auto zoneBFS = [&](int3 start, int3 goal) -> vector<int3> {
        for (int x = 0; x < mapWidth; x++)
            for (int y = 0; y < mapHeight; y++) {
                bfsVis[x][y] = {false, false};
                bfsPar[x][y] = {BFSState{-1, -1, -1, -1}, BFSState{-1, -1, -1, -1}};
            }

        int startPhase = (map.getTile(start)->getZoneID() == destZoneID) ? 1 : 0;
        bfsVis[start.x][start.y][startPhase] = true;

        priority_queue<BFSState> q;
        q.push({0, 0, start.x, start.y, startPhase});

        while (!q.empty()) {
            auto [currentType, currentDistance, currentX, currentY, currentPhase] = q.top();
            q.pop();

            if (currentX == goal.x && currentY == goal.y && currentPhase == 1) {
                vector<int3> path;
                BFSState state{currentType, currentDistance, currentX, currentY, currentPhase};
                while (state.x != -1) {
                    path.push_back(int3(state.x, state.y, 0));
                    BFSState parentState = bfsPar[state.x][state.y][state.phase];
                    state                = parentState;
                }
                reverse(path.begin(), path.end());
                return path;
            }

            for (int i = 0; i < 4; i++) {
                int3 neighbour = int3(currentX, currentY, 0) + directions4[i];
                if (!isInside(0, 0, mapWidth, mapHeight, neighbour))
                    continue;
                if (state[neighbour.x][neighbour.y] == 1)
                    continue;
                int nbZone = map.getTile(neighbour)->getZoneID();
                int neighbourPhase;
                if (currentPhase == 1) {
                    if (nbZone != destZoneID)
                        continue;
                    neighbourPhase = 1;
                } else {
                    neighbourPhase = (nbZone == destZoneID) ? 1 : 0;
                }
                if (!bfsVis[neighbour.x][neighbour.y][neighbourPhase]) {
                    bfsVis[neighbour.x][neighbour.y][neighbourPhase] = true;
                    bfsPar[neighbour.x][neighbour.y][neighbourPhase] = {
                        currentType, currentDistance, currentX, currentY, currentPhase};
                    int neighbourDistance = currentDistance + 1;
                    int neighbourType     = state[neighbour.x][neighbour.y];
                    q.push({neighbourType, neighbourDistance, neighbour.x, neighbour.y,
                            neighbourPhase});
                }
            }
        }
        return {};
    };

    vector<int3> witness = zoneBFS(fromPos, destPos);
    if (witness.empty())
        return witness;

    vector<vector<bool>> inWitness(mapWidth, vector<bool>(mapHeight, false));
    for (const auto &p : witness)
        inWitness[p.x][p.y] = true;

    vector<int3> openCells;
    for (int x = 0; x < mapWidth; x++)
        for (int y = 0; y < mapHeight; y++)
            if (state[x][y] == 0 && (map.getTile(int3(x, y, 0))->getZoneID() == fromZoneID ||
                                     map.getTile(int3(x, y, 0))->getZoneID() == destZoneID))
                openCells.push_back(int3(x, y, 0));

    int maxNumberOfNoice = openCells.size() * 0.3;
    while (!openCells.empty()) {
        maxNumberOfNoice--;
        if (maxNumberOfNoice <= 0)
            break;
        int idx        = rng.nextInt(0, (int)openCells.size() - 1);
        int3 c         = openCells[idx];
        openCells[idx] = openCells.back();
        openCells.pop_back();

        state[c.x][c.y] = 1;

        if (inWitness[c.x][c.y]) {
            inWitness[c.x][c.y]  = false;
            vector<int3> newPath = zoneBFS(fromPos, destPos);
            if (newPath.empty()) {
                state[c.x][c.y]     = 2;
                inWitness[c.x][c.y] = true;

                forcedCount++;
                if (forcedCount >= TRIES + 2)
                    break;
            } else {
                for (const auto &p : witness)
                    inWitness[p.x][p.y] = false;
                witness = newPath;
                for (const auto &p : witness)
                    inWitness[p.x][p.y] = true;
            }
        }
    }

    return witness;
}

std::map<int, vector<pair<int, int3>>> ConnectionPlacer::getConnectionsPoints() {
    ObjectVector objectVector = map.getObjectVector();
    std::map<int, vector<pair<int, int3>>> connectionsPoints;

    for (const auto &object : objectVector) {
        if (auto townPtr = dynamic_pointer_cast<Town>(object)) {

            // if (townPtr->getOwner() <= 0)
            //     continue;

            int3 townPos  = townPtr->getPosition();
            int3 townSize = townPtr->getSize();
            int3 connectionPointPos(townPos.x - townSize.x / 2, townPos.y,
                                    townPos.z); // Use town's entrance as connection point

            int zoneID = map.getTile(townPos)->getZoneID();

            connectionsPoints[zoneID].push_back({(townPtr->getOwner() > 0), connectionPointPos});
        }
    }

    auto zoneMap = map.getZoneMap();
    for (auto [zoneID, zone] : zoneMap) {
        if (connectionsPoints.find(zoneID) == connectionsPoints.end()) {
            int3 zoneCenter = zone->getCenter();
            connectionsPoints[zoneID].push_back({0, zoneCenter});
        } else {
            // If there are multiple connection points for a zone, sort them so that owned towns are
            // preferred
            sort(connectionsPoints[zoneID].begin(), connectionsPoints[zoneID].end(),
                 [](const pair<int, int3> &a, const pair<int, int3> &b) {
                     return a.first > b.first; // Sort by ownership (owned first)
                 });
        }
    }
    return connectionsPoints;
}
