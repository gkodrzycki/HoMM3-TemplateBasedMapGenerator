#pragma once

#include "../../global/Global.hpp"
#include "../../mapInfo/Obstacle.hpp"
#include "../../mapInfo/Road.hpp"
#include "../../mapInfo/Town.hpp"
#include "../Map.hpp"

class Map;

class ObjectPlacer {
  public:
    ObjectPlacer(Map &map);

    void placeTowns();
    void placeBorders();
    void placeRoads();

    void expandBorderTiles(vector<int3> &borderTiles, int maxDepth = 3);
    vector<int3> getBorderTiles();
    vector<int3> createPath(int3 fromPos, int3 destPos);
    std::map<int, int3> getConnectionsPoints();

  private:
    Map &map;
};
