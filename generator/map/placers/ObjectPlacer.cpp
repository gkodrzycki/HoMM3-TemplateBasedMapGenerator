#include "./ObjectPlacer.hpp"

ObjectPlacer::ObjectPlacer(Map &map) : map(map) {}

void ObjectPlacer::placeTowns() {
    ZoneMap zoneMap = map.getZoneMap();

    for (auto [zoneID, zone] : zoneMap) {
        string zoneType = zone->getType();

        if (zoneType == "starting_zone") {
            Town town(zone->getFaction(), zone->getOwner(), zone->getCenter(), "Town");
            auto townPtr = make_shared<Town>(town);
            map.addObject(townPtr);
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

    for (int3 pos : borderTiles) {
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
            if (newPos.x < 0 || newPos.y < 0 || newPos.x >= mapWidth || newPos.y >= mapHeight)
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
    expandBorderTiles(borderTiles);

    for (int3 pos : borderTiles) {
        Obstacle obstacle("Pine Trees", pos, "Obstacle");
        auto obstaclePtr = make_shared<Obstacle>(obstacle);
        map.addObject(obstaclePtr);
    }
}
