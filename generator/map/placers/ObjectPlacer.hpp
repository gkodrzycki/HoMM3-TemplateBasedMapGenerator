#pragma once

#include "../../global/Global.hpp"
#include "../../mapInfo/Town.hpp"
#include "../Map.hpp"

class Map;

class ObjectPlacer {
  public:
    ObjectPlacer(Map &map);

    void placeTowns();

  private:
    Map &map;
};
