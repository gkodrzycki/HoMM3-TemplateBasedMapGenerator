#pragma once

#include "../../global/CreatureIds.hpp"
#include "../../global/Global.hpp"
#include "../../global/Random.hpp"
#include "../../map/Map.hpp"
#include "../../mapInfo/Creature.hpp"

class Map;

enum class GuardTypeHandler { MINE, BORDER, TREASURE, DEFAULT };

class GuardHandler {
  public:
    GuardHandler(Map &map);

    shared_ptr<Creature> createGuard(GuardTypeHandler type, int3 position, int guardValue = -1);

  private:
    Map &map;
    RNG &rng;

    int getGuardLevel(GuardTypeHandler type, string zoneStrength);
    int getGuardPower(GuardTypeHandler type, string zoneStrength);
    string getGuardCreatureId(GuardTypeHandler type, int level);
    pair<const string *, int> getCreatureArrayForLevel(int level, bool upgraded);
};
