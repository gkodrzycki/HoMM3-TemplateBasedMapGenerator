#pragma once

#include "../global/Global.hpp"

class ZoneBlueprint {
  public:
    ZoneBlueprint();

    string getType();
    string getRichness();
    bool getPlaceBasicMines();
    map<string, int> &getMines();

    void deserializeZoneBlueprint(const json &zone);
    void printZoneBlueprint();

  private:
    string type;
    string richness;

    bool placeBasicMines;
    map<string, int> mines;
};
