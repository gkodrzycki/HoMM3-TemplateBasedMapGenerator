#pragma once

#include "../../global/Global.hpp"
#include "../../mapInfo/Obstacle.hpp"
#include "../../mapInfo/Town.hpp"
#include "../Map.hpp"

class Map;

class ObjectPlacer {
  public:
    ObjectPlacer(Map &map);

    void placeTowns();

    void placeBorders();

    vector<int3> getBorderTiles();
    void expandBorderTiles(vector<int3> &borderTiles, int maxDepth = 3);

  private:
    Map &map;
};
