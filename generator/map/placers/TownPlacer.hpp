#pragma once

#include "../../global/Global.hpp"
#include "../../mapInfo/Town.hpp"
#include "../Map.hpp"

class Map;

class TownPlacer {
  public:
    TownPlacer(Map &map);

    void placeTowns();
    bool hasSafeMargin(int3 pos, int zoneID, int margin);
    void placeSpecificTowns(TownSettings townSettings, vector<string> &townTypes, int zoneID,
                            bool neutral, bool upgraded, vector<int3> &freeTiles,
                            vector<int3> &placedTowns);

  private:
    Map &map;
};
