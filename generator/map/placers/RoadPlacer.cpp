#include "./RoadPlacer.hpp"

RoadPlacer::RoadPlacer(Map &map) : map(map) {}

void RoadPlacer::placeRoads() {
    std::map<int, int3> connectionsPoints = getConnectionsPoints();

    auto connections = map.getLayoutInfo().getConnectionInfoList();
    for (auto connection : connections) {
        int3 fromPos = connectionsPoints[connection.getZoneFrom()];
        int3 destPos = connectionsPoints[connection.getZoneDest()];

        vector<int3> path = createPath(fromPos, destPos);

        Road road(1, path, "Road");
        auto roadPtr = make_shared<Road>(road);
        map.addRoad(roadPtr);

        for (const auto &pos : path) {
            map.getTile(pos)->setTileType(TileType::TILE_ROAD);
        }
    }
}

vector<int3> RoadPlacer::createPath(int3 fromPos, int3 destPos) {
    int mapWidth = map.getWidth(), mapHeight = map.getHeight();
    RNG &rng = map.getRNG();

    int fromZoneID = map.getTile(fromPos)->getZoneID();
    int destZoneID = map.getTile(destPos)->getZoneID();

    vector<vector<bool>> hardBlocked(mapWidth, vector<bool>(mapHeight, false));
    for (const auto &object : map.getObjectVector()) {
        if (auto town = dynamic_pointer_cast<Town>(object)) {
            int3 objPos  = object->getPosition();
            int3 objSize = object->getSize();
            // TODO: real town footprint
            for (int x = objPos.x - objSize.x + 1; x <= objPos.x; x++) {
                for (int y = objPos.y - objSize.y + 1; y <= objPos.y; y++) {
                    if (isInside(0, 0, mapWidth, mapHeight, int3(x, y, 0)))
                        hardBlocked[x][y] = true;
                }
            }
        }
    }
    if (isInside(0, 0, mapWidth, mapHeight, fromPos))
        hardBlocked[fromPos.x][fromPos.y] = false;
    if (isInside(0, 0, mapWidth, mapHeight, destPos))
        hardBlocked[destPos.x][destPos.y] = false;

    // Chiseled paths state: 0=OPEN, 1=BLOCKED, 2=FORCED
    vector<vector<int>> state(mapWidth, vector<int>(mapHeight, 0));
    for (int x = 0; x < mapWidth; x++) {
        for (int y = 0; y < mapHeight; y++) {
            int3 p(x, y, 0);
            if (hardBlocked[x][y]) {
                state[x][y] = 1;
                continue;
            }
            auto tile = map.getTile(p);
            if (!tile || tile->isTileType("T")) {
                state[x][y] = 1;
                continue;
            }
            if (tile->getZoneID() != fromZoneID && tile->getZoneID() != destZoneID)
                state[x][y] = 1;
        }
    }
    state[fromPos.x][fromPos.y] = 2;
    state[destPos.x][destPos.y] = 2;

    auto neighbors4 = [&](const int3 &p) {
        std::array<int3, 4> out;
        for (int i = 0; i < 4; i++)
            out[i] = p + directions4[i];
        return out;
    };
    auto passable = [&](const int3 &p) {
        if (!isInside(0, 0, mapWidth, mapHeight, p))
            return false;
        return state[p.x][p.y] != 1;
    };

    vector<int3> witness = bfs_path_xy(mapWidth, mapHeight, fromPos, destPos, neighbors4, passable);
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
            inWitness[c.x][c.y] = false;
            vector<int3> newPath =
                bfs_path_xy(mapWidth, mapHeight, fromPos, destPos, neighbors4, passable);
            if (newPath.empty()) {
                state[c.x][c.y]     = 2;
                inWitness[c.x][c.y] = true;
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
