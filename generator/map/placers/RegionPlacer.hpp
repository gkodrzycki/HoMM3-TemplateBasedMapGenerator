#pragma once

#include "../../global/Global.hpp"
#include "../Map.hpp"

class Map;

class RegionPlacer {
  public:
    RegionPlacer(Map &map);

    void generateRegions();

  private:
    Map &map;
};