#pragma once

#include "../../global/Global.hpp"
#include "../../mapInfo/Town.hpp"
#include "../Map.hpp"

class Map;

class GuardPlacer {
  public:
    GuardPlacer(Map &map);

    void placeGuards();
    void placeBorderGuards();

  private:
    Map &map;
};
