#pragma once

#include "../../global/Global.hpp"
#include "../../global/GridSearch.hpp"
#include "../../mapInfo/Road.hpp"
#include "../Map.hpp"

class Map;

class RoadPlacer {
  public:
    RoadPlacer(Map &map);

    void placeRoads();
    vector<int3> createPath(int3 fromPos, int3 destPos);
    std::map<int, int3> getConnectionsPoints();

  private:
    Map &map;
};
