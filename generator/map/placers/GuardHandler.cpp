#include "./GuardHandler.hpp"

GuardHandler::GuardHandler(RNG &rng) : rng(rng) {}

int GuardHandler::getGuardLevel(GuardTypeHandler type) {
    auto &rng = this->rng;
    if (type == GuardTypeHandler::MINE) {
        return rng.nextInt(1, 2);
    } else if (type == GuardTypeHandler::BORDER) {
        return rng.nextInt(6, 7);
    } else {
        return 1;
    }
}

int GuardHandler::getGuardPower(GuardTypeHandler type) {
    if (type == GuardTypeHandler::MINE) {
        return 4000;
    } else if (type == GuardTypeHandler::BORDER) {
        return 40000;
    } else {
        return 1;
    }
}

pair<const string *, int> GuardHandler::getCreatureArrayForLevel(int level, bool upgraded) {
    switch (level) {
    case 1:
        return {upgraded ? upgradedLvl1CreatureIds : basicLvl1CreatureIds,
                upgraded ? sizeof(upgradedLvl1CreatureIds) / sizeof(upgradedLvl1CreatureIds[0])
                         : sizeof(basicLvl1CreatureIds) / sizeof(basicLvl1CreatureIds[0])};
    case 2:
        return {upgraded ? upgradedLvl2CreatureIds : basicLvl2CreatureIds,
                upgraded ? sizeof(upgradedLvl2CreatureIds) / sizeof(upgradedLvl2CreatureIds[0])
                         : sizeof(basicLvl2CreatureIds) / sizeof(basicLvl2CreatureIds[0])};
    case 3:
        return {upgraded ? upgradedLvl3CreatureIds : basicLvl3CreatureIds,
                upgraded ? sizeof(upgradedLvl3CreatureIds) / sizeof(upgradedLvl3CreatureIds[0])
                         : sizeof(basicLvl3CreatureIds) / sizeof(basicLvl3CreatureIds[0])};
    case 4:
        return {upgraded ? upgradedLvl4CreatureIds : basicLvl4CreatureIds,
                upgraded ? sizeof(upgradedLvl4CreatureIds) / sizeof(upgradedLvl4CreatureIds[0])
                         : sizeof(basicLvl4CreatureIds) / sizeof(basicLvl4CreatureIds[0])};
    case 5:
        return {upgraded ? upgradedLvl5CreatureIds : basicLvl5CreatureIds,
                upgraded ? sizeof(upgradedLvl5CreatureIds) / sizeof(upgradedLvl5CreatureIds[0])
                         : sizeof(basicLvl5CreatureIds) / sizeof(basicLvl5CreatureIds[0])};
    case 6:
        return {upgraded ? upgradedLvl6CreatureIds : basicLvl6CreatureIds,
                upgraded ? sizeof(upgradedLvl6CreatureIds) / sizeof(upgradedLvl6CreatureIds[0])
                         : sizeof(basicLvl6CreatureIds) / sizeof(basicLvl6CreatureIds[0])};
    case 7:
        return {upgraded ? upgradedLvl7CreatureIds : basicLvl7CreatureIds,
                upgraded ? sizeof(upgradedLvl7CreatureIds) / sizeof(upgradedLvl7CreatureIds[0])
                         : sizeof(basicLvl7CreatureIds) / sizeof(basicLvl7CreatureIds[0])};
    default:
        return {nullptr, 0};
    }
}

string GuardHandler::getGuardCreatureId(GuardTypeHandler type, int level) {
    bool upgraded =
        (type == GuardTypeHandler::BORDER); // Currently only border guards can be upgraded
    auto [creatureArray, arraySize] = getCreatureArrayForLevel(level, upgraded);

    if (creatureArray == nullptr || arraySize == 0) {
        throw runtime_error("Failed to find guard creature id");
    }

    auto &rng       = this->rng;
    int randomIndex = rng.nextInt(0, arraySize - 1);
    return creatureArray[randomIndex];
}

shared_ptr<Creature> GuardHandler::createGuard(GuardTypeHandler type, int3 position) {
    int level         = getGuardLevel(type);
    string creatureId = getGuardCreatureId(type, level);
    int targetPower   = getGuardPower(type);

    ifstream file("stats/CRTRAITS.json");
    if (!file.is_open()) {
        cerr << "Failed to open CRTRAITS.json file." << endl;
        return nullptr;
    }

    json traits;
    try {
        file >> traits;
        file.close();
    } catch (const json::parse_error &e) {
        cerr << "JSON parsing error: " << e.what() << endl;
        return nullptr;
    }

    int creatureFightValue = 100;
    for (const auto &entry : traits) {
        if (entry.is_object()) {
            string singular = getOrDefault<string>(entry, "singular", "");

            string searchName = creatureId;
            transform(searchName.begin(), searchName.end(), searchName.begin(), ::tolower);
            searchName[0] = toupper(searchName[0]);

            for (size_t i = 1; i < searchName.length(); i++) {
                if (searchName[i - 1] == ' ') {
                    searchName[i] = toupper(searchName[i]);
                }
            }

            if (singular == searchName) {
                creatureFightValue = getOrDefault<int>(entry, "fight_value", 100);
                break;
            }
        }
    }

    int count = max(1, targetPower / creatureFightValue);

    Creature guard = Creature(getEnumFromNameOrThrow<CreatureType>(creatureId), position, count,
                              "AGGRESSIVE", true, false, "Creature");
    return make_shared<Creature>(guard);
}
