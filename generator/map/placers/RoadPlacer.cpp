#include "./RoadPlacer.hpp"

RoadPlacer::RoadPlacer(Map &map) : map(map) {}

void RoadPlacer::placeRoads() {
    std::map<int, int3> connectionsPoints = getConnectionsPoints();

    auto connections = map.getLayoutInfo().getConnectionInfoList();
    for (auto connection : connections) {
        int3 fromPos = connectionsPoints[connection.getZoneFrom()];
        int3 destPos = connectionsPoints[connection.getZoneDest()];

        vector<int3> path = createPath(fromPos, destPos);

        if (path.empty()) {
            throw runtime_error("Failed to create a path between zones " +
                                to_string(connection.getZoneFrom()) + " and " +
                                to_string(connection.getZoneDest()) + " on seed " +
                                to_string(map.getRNG().getOriginalSeed()) + "\n");
        }

        Road road(1, path, "Road");
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

vector<int3> RoadPlacer::createPath(int3 fromPos, int3 destPos) {
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
        int type, x, y, phase;
        bool operator<(const BFSState &o) const {
            return type < o.type; // max-heap: prefer type 2 (FORCED) over 0 (OPEN)
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
        q.push({0, start.x, start.y, startPhase});

        while (!q.empty()) {
            auto [ct, cx, cy, cp] = q.top();
            q.pop();

            if (cx == goal.x && cy == goal.y && cp == 1) {
                vector<int3> path;
                BFSState s{ct, cx, cy, cp};
                while (s.x != -1) {
                    path.push_back(int3(s.x, s.y, 0));
                    BFSState p = bfsPar[s.x][s.y][s.phase];
                    s          = p;
                }
                reverse(path.begin(), path.end());
                return path;
            }

            for (int i = 0; i < 4; i++) {
                int3 nb = int3(cx, cy, 0) + directions4[i];
                if (!isInside(0, 0, mapWidth, mapHeight, nb))
                    continue;
                if (state[nb.x][nb.y] == 1)
                    continue;
                int nbZone = map.getTile(nb)->getZoneID();
                int np;
                if (nbZone != destZoneID && nbZone != fromZoneID)
                    continue;
                if (cp == 1) {
                    if (nbZone != destZoneID)
                        continue;
                    np = 1;
                } else {
                    np = (nbZone == destZoneID) ? 1 : 0;
                }
                if (!bfsVis[nb.x][nb.y][np]) {
                    bfsVis[nb.x][nb.y][np] = true;
                    bfsPar[nb.x][nb.y][np] = {ct, cx, cy, cp};
                    int nt                 = state[nb.x][nb.y];
                    q.push({nt, nb.x, nb.y, np});
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
            if (state[x][y] == 0)
                openCells.push_back(int3(x, y, 0));

    while (!openCells.empty()) {
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

std::map<int, int3> RoadPlacer::getConnectionsPoints() {
    ObjectVector objectVector = map.getObjectVector();
    std::map<int, int3> connectionsPoints;

    for (const auto &object : objectVector) {
        if (auto townPtr = dynamic_pointer_cast<Town>(object)) {
            int3 townPos  = townPtr->getPosition();
            int3 townSize = townPtr->getSize();
            int3 connectionPointPos(townPos.x - townSize.x / 2, townPos.y,
                                    townPos.z); // Use town's entrance as connection point

            int zoneID = map.getTile(townPos)->getZoneID();

            connectionsPoints[zoneID] = connectionPointPos;
        }
    }

    auto zoneMap = map.getZoneMap();
    for (auto [zoneID, zone] : zoneMap) {
        if (connectionsPoints.find(zoneID) == connectionsPoints.end()) {
            int3 zoneCenter           = zone->getCenter();
            connectionsPoints[zoneID] = zoneCenter;
        }
    }
    return connectionsPoints;
}
