#pragma once

#include "../../global/CreatureIds.hpp"
#include "../../global/Global.hpp"
#include "../../global/Random.hpp"
#include "../../mapInfo/Town.hpp"
#include "../Map.hpp"

class Map;

enum class GuardType { MINE, BORDER, DEFAULT };

class GuardPlacer {
  public:
    GuardPlacer(Map &map);

    void placeGuards();
    void placeBorderGuards();
    shared_ptr<Creature> createGuard(GuardType type, int3 position);

  private:
    Map &map;

    int getGuardLevel(GuardType type);
    int getGuardPower(GuardType type);
    string getGuardCreatureId(GuardType type, int level);
    pair<const string *, int> getCreatureArrayForLevel(int level, bool upgraded);
};
