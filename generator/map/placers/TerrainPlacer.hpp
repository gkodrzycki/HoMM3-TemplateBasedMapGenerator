#pragma once

#include "../../global/CellularAutomata.hpp"
#include "../Map.hpp"

class Map;

class TerrainPlacer {
  public:
    TerrainPlacer(Map &map);
    void generateNoise();
    void placeObstacles();

  private:
    Map &map;
};
