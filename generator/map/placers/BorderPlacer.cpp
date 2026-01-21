#include "./BorderPlacer.hpp"

BorderPlacer::BorderPlacer(Map &map) : map(map) {}

vector<int3> BorderPlacer::getBorderTiles() {
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

void BorderPlacer::expandBorderTiles(vector<int3> &borderTiles, int maxDepth) {
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

void BorderPlacer::placeBorders() {
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
