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

                if (neighTilePtr->getZoneID() != currentTilePtr->getZoneID()) {
                    borderTiles.push_back(currentPos);
                    break;
                }
            }
        }
    }

    return borderTiles;
}

void BorderPlacer::expandBorderTiles(vector<int3> &borderTiles, int maxDepth) {
    int mapWidth = map.getWidth(), mapHeight = map.getHeight();

    auto neighbors8 = [&](const int3 &p) {
        std::array<int3, 8> out;
        for (int i = 0; i < 8; i++)
            out[i] = p + directions8[i];
        return out;
    };

    auto passable = [&](const int3 &p) { return true; };

    borderTiles =
        bfs_collect_depth_xy(mapWidth, mapHeight, borderTiles, maxDepth, neighbors8, passable);
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
