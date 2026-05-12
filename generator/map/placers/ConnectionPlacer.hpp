#pragma once

#include "../../global/Global.hpp"
#include "../../global/GridSearch.hpp"
#include "../../mapInfo/Road.hpp"
#include "../Map.hpp"

class Map;

class ConnectionPlacer {
  public:
    ConnectionPlacer(Map &map);

    void placeRoads();
    void createMonoliths();

    vector<int3> createPath(int3 fromPos, int3 destPos);
    std::map<int, vector<pair<int, int3>>> getConnectionsPoints();

  private:
    Map &map;
};
