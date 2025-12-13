#pragma once

#include "../../global/Global.hpp"
#include "../Map.hpp"

class Map;

class RegionPlacer {
  public:
    RegionPlacer(Map &map);

    void generateRegions();

    void placeRegions();

    void claimTiles(vector<pair<int, int3>> &zoneCenters);

  private:
    Map &map;
};