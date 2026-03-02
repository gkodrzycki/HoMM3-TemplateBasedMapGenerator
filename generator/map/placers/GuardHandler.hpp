#pragma once

#include "../../global/CreatureIds.hpp"
#include "../../global/Global.hpp"
#include "../../global/Random.hpp"
#include "../../mapInfo/Creature.hpp"

class Map;

enum class GuardTypeHandler { MINE, BORDER, DEFAULT };

class GuardHandler {
  public:
    GuardHandler(RNG &rng);
    shared_ptr<Creature> createGuard(GuardTypeHandler type, int3 position);

  private:
    RNG &rng;

    int getGuardLevel(GuardTypeHandler type);
    int getGuardPower(GuardTypeHandler type);
    string getGuardCreatureId(GuardTypeHandler type, int level);
    pair<const string *, int> getCreatureArrayForLevel(int level, bool upgraded);
};
