#pragma once

#include "../../global/Global.hpp"
#include "../../global/GridSearch.hpp"
#include "../Map.hpp"

class Map;

using DistanceMap = std::map<int, unordered_map<int, int>>;

class ZonePlacer {
  public:
    ZonePlacer(Map &map);

    void initZones();
    void generateZones();

    int calculateTotalSize();

    void placeZones();

    void calculateZoneCenters();

    void claimAbstractTile(int zoneID, int3 zoneCenter);

    void initGrid();

    void claimTiles(vector<pair<int, int3>> &zoneTiles);
    void claimFreeTiles();

    void calculateDistances();

    int getPercentageSize(int zoneSize, int totalSize);

    void generateAbstractGrid();
    bool validateAbstractGrid();

    void placeAbstractGridOnRealMap();

  private:
    Map &map;
    DistanceMap distancedBetweenZones;
    int gridN;
    vector<vector<int>> grid;
};
