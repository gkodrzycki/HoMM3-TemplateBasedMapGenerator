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
    auto neighbors8 = [&](const int3 &p) {
        std::array<int3, 8> out;
        for (int i = 0; i < 8; i++)
            out[i] = p + directions8[i];
        return out;
    };

    auto passable = [&](const int3 &p) { return true; };

    borderTiles =
        bfs_collect_depth_xy(map.getSearchCtx(), borderTiles, maxDepth, neighbors8, passable);
}

void BorderPlacer::reserveBorderTiles() {
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
}

void BorderPlacer::placeBorders() {
    int mapWidth = map.getWidth(), mapHeight = map.getHeight();

    for (int x = 0; x < mapWidth; x++) {
        for (int y = 0; y < mapHeight; y++) {
            int3 currentPos(x, y, 0);
            auto currentTilePtr = map.getTile(currentPos);

            if (currentTilePtr->isTileType("Bb")) {
                Obstacle obstacle("Pine Trees", currentPos, "Obstacle");
                auto obstaclePtr = make_shared<Obstacle>(obstacle);
                map.addObject(obstaclePtr);
            }
        }
    }
}
