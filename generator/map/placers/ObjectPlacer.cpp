#include "./ObjectPlacer.hpp"

ObjectPlacer::ObjectPlacer(Map &map) : map(map) {}

void ObjectPlacer::placeTowns() {
    ZoneMap zoneMap = map.getZoneMap();
    int3 offset(0, 0, 0);

    for (auto [zoneID, zone] : zoneMap) {
        string zoneType = zone->getType();

        if (zoneType == "starting_zone") {
            Town town(zone->getFaction(), zone->getOwner(), zone->getCenter(), "Town");
            auto townPtr = make_shared<Town>(town);
            map.addObject(townPtr);

            fixNeighbourTiles(townPtr->getPosition(), townPtr->getSize(), zoneID, offset);
            int3 entrancePos = townPtr->getPosition() - int3(townPtr->getSize().x / 2, 0, 0);
            map.getTile(entrancePos)->setTileType(TileType::TILE_OCCUPIED);
        }
    }
}

vector<int3> ObjectPlacer::getBorderTiles() {
    int mapWidth = map.getWidth(), mapHeight = map.getHeight();
    vector<int3> borderTiles;

    for (int x = 0; x < mapWidth; x++) {
        for (int y = 0; y < mapHeight; y++) {
            int3 currentPos(x, y, 0);
            auto currentTilePtr = map.getTile(currentPos);

            for (int3 dir : directions8) {
                int3 neighPos = currentPos + dir;

                auto neighTilePtr = map.getTile(neighPos);
                if (neighTilePtr == nullptr)
                    continue;

                if (neighTilePtr->getTerrain() != currentTilePtr->getTerrain()) {
                    borderTiles.push_back(currentPos);
                    break;
                }
            }
        }
    }

    return borderTiles;
}

void ObjectPlacer::expandBorderTiles(vector<int3> &borderTiles, int maxDepth) {
    queue<pair<int, int3>> q;
    set<int3> visited;
    int mapWidth = map.getWidth(), mapHeight = map.getHeight();

    for (const int3 &pos : borderTiles) {
        q.push({0, pos});
        visited.insert(pos);
    }

    while (q.size()) {
        auto [depth, currentPos] = q.front();
        q.pop();

        if (depth >= maxDepth)
            continue;

        for (auto direction : directions8) {
            int3 newPos = currentPos + direction;
            if (!isInside(0, 0, mapWidth, mapHeight, newPos))
                continue;

            if (visited.find(newPos) != visited.end())
                continue;

            borderTiles.push_back(newPos);
            visited.insert(newPos);
            q.push({depth + 1, newPos});
        }
    }
}

void ObjectPlacer::placeBorders() {
    vector<int3> borderTiles = getBorderTiles();

    for (const int3 &pos : borderTiles) {
        if (map.getTile(pos)->isTileType("OTRr"))
            continue;
        map.getTile(pos)->setTileType(TileType::TILE_BORDER_INNER);
    }

    expandBorderTiles(borderTiles);

    for (const int3 &pos : borderTiles) {
        if (map.getTile(pos)->isTileType("BOTRr"))
            continue;
        map.getTile(pos)->setTileType(TileType::TILE_BORDER_OUTER);
    }

    for (const int3 &pos : borderTiles) {

        if (map.getTile(pos)->isTileType("OTRr"))
            continue;

        auto upperTile = map.getTile(pos + int3(0, -1, 0));
        if (upperTile != nullptr && upperTile->isTileType("r"))
            continue;

        Obstacle obstacle("Pine Trees", pos, "Obstacle");
        auto obstaclePtr = make_shared<Obstacle>(obstacle);
        map.addObject(obstaclePtr);
    }
}

std::map<int, int3> ObjectPlacer::getConnectionsPoints() {
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

vector<int3> ObjectPlacer::createPath(int3 fromPos, int3 destPos) {
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

void ObjectPlacer::placeRoads() {
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

void ObjectPlacer::fixNeighbourTiles(const int3 &pos, const int3 &size, int zoneID,
                                     const int3 &offset) {
    auto zoneMap       = map.getZoneMap();
    string zoneTerrain = zoneMap[zoneID]->getTerrain();

    for (int dx = 0; dx < size.x + offset.x; dx++) {
        for (int dy = 0; dy < size.y + offset.y; dy++) {
            int3 tilePos(pos.x - size.x + dx + 1, pos.y - size.y + dy + 1, pos.z);
            auto tilePtr = map.getTile(tilePos);

            if (tilePtr == nullptr)
                continue;

            if (offset.x >= 1 && offset.y >= 1 &&
                (dx == size.x + offset.x - 1 || dy == size.y + offset.y - 1)) {
                tilePtr->setTileType(TileType::TILE_RESERVED);
            } else {
                tilePtr->setTileType(TileType::TILE_TAKEN);
            }
            tilePtr->setZoneID(zoneID);
            tilePtr->setTerrain(zoneTerrain);
        }
    }
}

bool ObjectPlacer::checkPlacementConflict(const int3 &pos, const int3 &size, const string &types) {
    auto zoneMap = map.getZoneMap();

    for (int dx = 0; dx < size.x; dx++) {
        for (int dy = 0; dy < size.y; dy++) {
            int3 tilePos(pos.x - size.x + dx + 1, pos.y - size.y + dy + 1, pos.z);
            auto tilePtr = map.getTile(tilePos);

            if (tilePtr == nullptr)
                continue;

            if (tilePtr->isTileType(types)) {
                return true;
            }
        }
    }
    return false;
}

void ObjectPlacer::placeResource(ResourceType resourceType, int3 pos, int quantity) {
    if (quantity <= 0)
        return;

    Resource resource(resourceType, quantity, pos, "Resource");
    auto resourcePtr = make_shared<Resource>(resource);
    map.addResource(resourcePtr);
}

void ObjectPlacer::placeCreature(CreatureType creatureType, int3 pos, int quantity) {
    if (quantity <= 0)
        return;

    Creature creature(creatureType, pos, quantity, "COMPLIANT", true, true, "Creature");
    auto creaturePtr = make_shared<Creature>(creature);
    map.addCreature(creaturePtr);
}

void ObjectPlacer::placeBasicMines() {
    auto objectVector = map.getObjectVector();

    int mapWidth  = map.getWidth();
    int mapHeight = map.getHeight();

    for (const auto &object : objectVector) {
        if (auto town = dynamic_pointer_cast<Town>(object)) {
            int3 anchorPoint = object->getPosition();
            anchorPoint.x -= 2;

            pair<int3, int3> BC;
            int maxIter = 100;
            while (maxIter--) {

                BC          = map.getRNG().getRandomTriangle(anchorPoint, 30);
                auto [B, C] = BC;

                if (!isInside(2, 2, mapWidth - 2, mapHeight - 2, B))
                    continue;
                if (!isInside(2, 2, mapWidth - 2, mapHeight - 2, C))
                    continue;

                if (map.getTile(B)->getZoneID() != map.getTile(anchorPoint)->getZoneID())
                    continue;
                if (map.getTile(C)->getZoneID() != map.getTile(anchorPoint)->getZoneID())
                    continue;

                if (anchorPoint.distance2DSQ(B) <= 5)
                    continue;
                if (anchorPoint.distance2DSQ(C) <= 5)
                    continue;
                if (B.distance2DSQ(C) <= 3)
                    continue;

                if (checkPlacementConflict(B, getMineSize(MineType::MINE_SAWMILL)))
                    continue;

                if (checkPlacementConflict(C, getMineSize(MineType::MINE_ORE_PIT)))
                    continue;
                // if (B and C inside and good)
                break;
            }

            auto [B, C] = BC;

            int anchorZoneID                  = map.getTile(anchorPoint)->getZoneID();
            array<int, 4> &basicResourceCount = map.getBasicResourceCount();
            int3 left                         = int3(-2, 1, 0);
            int3 right                        = int3(0, 1, 0);
            int3 down                         = int3(-1, 1, 0);

            Mine mineSawmill(MineType::MINE_SAWMILL, -1, B, "Mine");
            auto mineSawmillPtr = make_shared<Mine>(mineSawmill);
            map.addObject(mineSawmillPtr);
            fixNeighbourTiles(B, getMineSize(mineSawmill.getMineType()), anchorZoneID);
            placeResource(ResourceType::RESOURCE_WOOD, B + left, basicResourceCount[0]);
            placeResource(ResourceType::RESOURCE_WOOD, B + right, basicResourceCount[1]);
            placeCreature(CreatureType::PIKEMAN, B + down, 5);

            Mine mineOrePit(MineType::MINE_ORE_PIT, -1, C, "Mine");
            auto mineOrePitPtr = make_shared<Mine>(mineOrePit);
            map.addObject(mineOrePitPtr);
            fixNeighbourTiles(C, getMineSize(mineOrePit.getMineType()), anchorZoneID);
            placeResource(ResourceType::RESOURCE_ORE, C + left, basicResourceCount[2]);
            placeResource(ResourceType::RESOURCE_ORE, C + right, basicResourceCount[3]);
            placeCreature(CreatureType::PIKEMAN, C + down, 5);
        }
    }
}
