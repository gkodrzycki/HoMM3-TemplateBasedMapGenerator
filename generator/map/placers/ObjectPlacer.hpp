#pragma once

#include "../../global/Global.hpp"
#include "../../mapInfo/Creature.hpp"
#include "../../mapInfo/Mine.hpp"
#include "../../mapInfo/Obstacle.hpp"
#include "../../mapInfo/Resource.hpp"
#include "../Map.hpp"

class Map;

class ObjectPlacer {
  public:
    ObjectPlacer(Map &map);

    void placeBasicMines();

  private:
    Map &map;

    void placeResource(ResourceType resourceType, int3 pos, int quantity);
    void placeCreature(CreatureType creatureType, int3 pos, int quantity);
};
