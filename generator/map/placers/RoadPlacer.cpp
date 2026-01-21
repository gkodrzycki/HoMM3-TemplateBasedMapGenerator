#include "./RoadPlacer.hpp"

RoadPlacer::RoadPlacer(Map &map) : map(map) {}

void RoadPlacer::placeRoads() {
    std::map<int, int3> connectionsPoints = getConnectionsPoints();

    auto connections = map.getLayoutInfo().getConnectionInfoList();
    for (auto connection : connections) {
        int3 fromPos = connectionsPoints[connection.getZoneFrom()];
        int3 destPos = connectionsPoints[connection.getZoneDest()];

        vector<int3> path = createPath(fromPos, destPos);

        for (const auto &pos : path) {
            Road road(1, pos, "Road");
            auto roadPtr = make_shared<Road>(road);
            map.addObject(roadPtr);
            map.getTile(pos)->setTileType(TileType::TILE_ROAD);
        }
    }
}

vector<int3> RoadPlacer::createPath(int3 fromPos, int3 destPos) {
    int mapWidth = map.getWidth(), mapHeight = map.getHeight();

    vector<vector<int3>> visited(mapWidth, vector<int3>(mapHeight, int3(-1, -1, -1)));
    for (int x = 0; x < mapWidth; x++) {
        for (int y = 0; y < mapHeight; y++) {
            visited[x][y] = int3(-1, -1, -1);
        }
    }

    auto objectVector = map.getObjectVector();
    for (const auto &object : objectVector) {
        if (auto town = dynamic_pointer_cast<Town>(object)) {
            int3 objPos  = object->getPosition();
            int3 objSize = object->getSize();

            // TODO: use real Town size here
            for (int x = objPos.x - objSize.x + 1; x <= objPos.x; x++) {
                for (int y = objPos.y - objSize.y + 1; y <= objPos.y; y++) {
                    if (!isInside(0, 0, mapWidth, mapHeight, int3(x, y, 0)))
                        continue;

                    visited[x][y] = int3(-2, -2, -2);
                }
            }
        }
    }

    queue<int3> q;
    q.push(fromPos);
    visited[fromPos.x][fromPos.y] = fromPos;
    visited[destPos.x][destPos.y] = int3(-1, -1, -1);

    while (q.size()) {
        int3 currentPos = q.front();
        q.pop();

        if (currentPos == destPos) {
            break;
        }

        for (auto direction : directions4) {
            int3 newPos = currentPos + direction;
            if (!isInside(0, 0, mapWidth, mapHeight, newPos))
                continue;

            auto tilePtr = map.getTile(newPos);
            if (tilePtr == nullptr)
                continue;

            if (tilePtr->isTileType("T"))
                continue;

            if (visited[newPos.x][newPos.y] != int3(-1, -1, -1) ||
                visited[newPos.x][newPos.y] == int3(-2, -2, -2))
                continue;

            visited[newPos.x][newPos.y] = currentPos;
            q.push(newPos);
        }
    }

    vector<int3> path;
    int3 tracePos = destPos;
    while (tracePos != fromPos) {
        path.push_back(tracePos);
        tracePos = visited[tracePos.x][tracePos.y];

        if (tracePos == int3(-1, -1, -1) || tracePos == int3(-2, -2, -2)) {
            cerr << "[ERROR] No valid path found from " << fromPos.toString() << " to "
                 << destPos.toString() << "\n";
            return vector<int3>();
        }
    }

    path.push_back(fromPos);
    reverse(path.begin(), path.end());

    return path;
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
