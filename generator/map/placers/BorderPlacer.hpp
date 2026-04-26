#pragma once

#include "../../global/Global.hpp"
#include "../../global/GridSearch.hpp"
#include "../../mapInfo/Obstacle.hpp"
#include "../../mapInfo/Town.hpp"
#include "../Map.hpp"

class Map;
class Obstacle;

class BorderPlacer {
  public:
    BorderPlacer(Map &map);

    void reserveBorderTiles();

    void placeBorders();
    void expandBorderTiles(vector<int3> &borderTiles, int maxDepth = 1);
    vector<int3> getBorderTiles();

  private:
    Map &map;
};
