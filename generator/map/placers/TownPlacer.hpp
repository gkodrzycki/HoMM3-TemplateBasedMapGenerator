#pragma once

#include "../../global/Global.hpp"
#include "../../mapInfo/Town.hpp"
#include "../Map.hpp"

class Map;

class TownPlacer {
  public:
    TownPlacer(Map &map);

    void placeTowns();
    void placeSpecificTowns(TownSettings townSettings, vector<string> &townTypes, int zoneID,
                            bool neutral, bool upgraded, vector<int3> &freeTiles,
                            vector<int3> &placedTowns);

  private:
    Map &map;
};
