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

    vector<vector<bool>> blocked(mapWidth, vector<bool>(mapHeight, false));

    for (const auto &object : map.getObjectVector()) {
        if (auto town = dynamic_pointer_cast<Town>(object)) {
            int3 objPos  = object->getPosition();
            int3 objSize = object->getSize();

            // TODO: real town footprint
            for (int x = objPos.x - objSize.x + 1; x <= objPos.x; x++) {
                for (int y = objPos.y - objSize.y + 1; y <= objPos.y; y++) {
                    if (!isInside(0, 0, mapWidth, mapHeight, int3(x, y, 0)))
                        continue;
                    blocked[x][y] = true;
                }
            }
        }
    }

    if (isInside(0, 0, mapWidth, mapHeight, fromPos))
        blocked[fromPos.x][fromPos.y] = false;
    if (isInside(0, 0, mapWidth, mapHeight, destPos))
        blocked[destPos.x][destPos.y] = false;

    auto neighbors4 = [&](const int3 &p) {
        std::array<int3, 4> out;
        for (int i = 0; i < 4; i++)
            out[i] = p + directions4[i];
        return out;
    };

    auto passable = [&](const int3 &p) {
        if (!isInside(0, 0, mapWidth, mapHeight, p))
            return false;
        if (blocked[p.x][p.y])
            return false;

        auto tile = map.getTile(p);
        if (!tile)
            return false;
        if (tile->isTileType("T"))
            return false;

        return true;
    };
    auto path = bfs_path_xy(mapWidth, mapHeight, fromPos, destPos, neighbors4, passable);

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
