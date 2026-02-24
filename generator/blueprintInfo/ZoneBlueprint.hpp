#pragma once

#include "../global/Global.hpp"
#include "../mapInfo/MineTypeInfo.hpp"

class ZoneBlueprint {
  public:
    ZoneBlueprint();

    string getType();
    string getRichness();
    bool getPlaceBasicMines();
    map<MineTypeInfo, int> &getMines();

    void deserializeZoneBlueprint(const json &zone);
    void printZoneBlueprint();

  private:
    string type;
    string richness;

    bool placeBasicMines;
    map<MineTypeInfo, int> mines;
};
