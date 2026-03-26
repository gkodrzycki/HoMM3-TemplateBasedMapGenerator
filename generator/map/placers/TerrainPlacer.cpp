#include "./TerrainPlacer.hpp"

TerrainPlacer::TerrainPlacer(Map &map) : map(map) {}

void TerrainPlacer::generateNoise() {
    int width  = map.getWidth();
    int height = map.getHeight();

    float initialFillProbability = 0.45f;
    int birthLimit               = 6;
    int survivalLimit            = 4;
    int smoothingIterations      = 2;

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
