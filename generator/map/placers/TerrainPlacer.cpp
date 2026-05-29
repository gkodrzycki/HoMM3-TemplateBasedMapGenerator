#include "./TerrainPlacer.hpp"

TerrainPlacer::TerrainPlacer(Map &map) : map(map) {}

void TerrainPlacer::generateNoise() {
    int width  = map.getWidth();
    int height = map.getHeight();

    float initialFillProbability = 0.45f;
    int birthLimit               = 5;
    int survivalLimit            = 4;
    int smoothingIterations      = 3;

    AutomataConfig config{width,      height,        initialFillProbability,
                          birthLimit, survivalLimit, smoothingIterations};
    CellularAutomata automata(config);
    automata.generate(this->map);
    auto grid = automata.getGrid();

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            auto tilePtr = map.getTile(int3(x, y, 0));
            if (tilePtr != nullptr) {
                if (grid[y * width + x]) {
                    if (tilePtr->isTileType("B"))
                        continue;
                    tilePtr->setTileType(TileType::TILE_OBSTACLE);
                }
            }
        }
    }

    fillClosedRooms();
}

void TerrainPlacer::fillClosedRooms() {
    int W = map.getWidth();
    int H = map.getHeight();

    auto passable   = [&](const int3 &pos) { return map.getTile(pos)->isTileType("F"); };
    auto neighbours = [&](const int3 &pos) {
        std::array<int3, 4> out;
        for (int i = 0; i < 4; i++)
            out[i] = pos + directions4[i];
        return out;
    };

    vector<vector<bool>> visited(W, vector<bool>(H, false));

    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            int3 pos(x, y, 0);
            if (!map.getTile(pos)->isTileType("F"))
                continue;
            if (visited[pos.x][pos.y])
                continue;

            auto component = bfs_claim_component(W, H, pos, neighbours, passable);
            if (component.size() <= 10) {
                for (const auto &p : component) {
                    map.getTile(p)->setTileType(TileType::TILE_OBSTACLE);
                }
            }
            for (const auto &p : component) {
                visited[p.x][p.y] = true;
            }
        }
    }
}

void TerrainPlacer::placeObstacles() {
    int width  = map.getWidth();
    int height = map.getHeight();

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            auto tilePtr = map.getTile(int3(x, y, 0));
            if (tilePtr != nullptr) {
                if (tilePtr->isTileType("B"))
                    continue;
                if (tilePtr->isTileType("O")) {
                    Obstacle obstacle("Oak Trees", int3(x, y, 0), "Obstacle");
                    auto obstaclePtr = make_shared<Obstacle>(obstacle);
                    map.addObject(obstaclePtr);
                }
            }
        }
    }
}
