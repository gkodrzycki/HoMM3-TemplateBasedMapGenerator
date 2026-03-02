#pragma once

#include "../global/Global.hpp"
#include "../map/placers/GuardHandler.hpp"
#include "../mapInfo/MineTypeInfo.hpp"

class ZoneBlueprint {
  public:
    ZoneBlueprint(RNG &rng);

    string getType();
    string getRichness();
    bool getPlaceBasicMines();
    map<MineTypeInfo, int> &getMines();
    pair<int, int> getRandomResourcesCount();
    map<MineTypeInfo, vector<pair<int, int>>> &getMineResourcesCount();
    map<MineTypeInfo, vector<Creature>> &getMineGuards();
    vector<MineTypeInfo> getRandomMineTypes();

    void deserializeZoneBlueprint(const json &zone);
    void printZoneBlueprint();

  private:
    RNG &rng;

    string type;
    string richness;

    bool placeBasicMines;
    map<MineTypeInfo, int> mines;
    map<MineTypeInfo, vector<pair<int, int>>> mineResourcesCount;
    map<MineTypeInfo, vector<Creature>> mineGuards;
    vector<MineTypeInfo> randomMineTypes;
};
