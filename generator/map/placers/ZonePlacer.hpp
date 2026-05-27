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
    void groupZones();
    void generateZones();

    int calculateTotalSize();

    void placeZones();

    void calculateZoneCenters();

    void claimAbstractTile(int zoneID, int3 zoneCenter);

    void initGrid();

    bool claimTiles(vector<pair<int, int3>> &zoneTiles, bool fullClaim);
    bool claimFreeTiles(bool fullClaim = false);

    void calculateDistances();

    int getPercentageSize(int zoneSize, int gridN, int totalSize);

    void generateAbstractGrid();
    bool validateAbstractGrid();

    void placeAbstractGridOnRealMap();
    string generateZoneHash(int zoneID);

  private:
    Map &map;
    DistanceMap distancedBetweenZones;
    GridSearchContext gridCtx;
    int gridN;
    vector<vector<int>> grid;
};
