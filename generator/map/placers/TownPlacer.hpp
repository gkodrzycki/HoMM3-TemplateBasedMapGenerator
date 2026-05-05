#pragma once

#include "../../global/Global.hpp"
#include "../../mapInfo/Town.hpp"
#include "../Map.hpp"

class Map;

class TownPlacer {
  public:
    TownPlacer(Map &map);

    void placeTowns();
    void placeSpecificTowns(int numberOfTowns, int zoneID, bool neutral, bool upgraded);

  private:
    Map &map;
};
