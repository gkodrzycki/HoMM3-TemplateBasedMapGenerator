#pragma once

#include "../../global/Global.hpp"
#include "../../global/GridSearch.hpp"
#include "../../mapInfo/Town.hpp"
#include "../Map.hpp"

class Map;

class BorderPlacer {
  public:
    BorderPlacer(Map &map);

    void placeBorders();
    void expandBorderTiles(vector<int3> &borderTiles, int maxDepth = 1);
    vector<int3> getBorderTiles();

  private:
    Map &map;
};
