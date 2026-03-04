#include "./ZoneBlueprint.hpp"

ZoneBlueprint::ZoneBlueprint(RNG &rng) : rng(rng), type("support_zone"), richness("high") {}

string ZoneBlueprint::getType() { return type; }
string ZoneBlueprint::getRichness() { return richness; }
bool ZoneBlueprint::getPlaceBasicMines() { return placeBasicMines; }
map<MineTypeInfo, int> &ZoneBlueprint::getMines() { return mines; }
map<MineTypeInfo, vector<pair<int, int>>> &ZoneBlueprint::getMineResourcesCount() {
    return mineResourcesCount;
}
map<MineTypeInfo, vector<Creature>> &ZoneBlueprint::getMineGuards() { return mineGuards; }
vector<MineTypeInfo> ZoneBlueprint::getRandomMineTypes() { return randomMineTypes; }

pair<int, int> ZoneBlueprint::getRandomResourcesCount() {
    if (richness == "low") {
        return {rng.nextInt(1, 3), rng.nextInt(1, 3)};
    }
    if (richness == "medium") {
        return {rng.nextInt(4, 6), rng.nextInt(4, 6)};
    }
    if (richness == "high") {
        return {rng.nextInt(7, 10), rng.nextInt(7, 10)};
    }

    throw runtime_error("Richness level '" + richness +
                        "' is not recognized. Valid values are: low, medium, high.");
}

void ZoneBlueprint::deserializeZoneBlueprint(const json &zone) {
    string type          = getOrError<string>(zone, "type");
    string richness      = getOrError<string>(zone, "richness");
    bool placeBasicMines = getOrDefault<bool>(zone, "placeBasicMines", true);

    this->type            = type;
    this->richness        = richness;
    this->placeBasicMines = placeBasicMines;

    const auto &mineList = getOrDefault<json>(zone, "mines", json::object());

    if (this->placeBasicMines) {
        this->mineResourcesCount[MineTypeInfo::SAWMILL].push_back(getRandomResourcesCount());
        this->mineResourcesCount[MineTypeInfo::ORE_PIT].push_back(getRandomResourcesCount());
        GuardHandler guardHandler(this->rng);
        auto guardPtr = guardHandler.createGuard(GuardTypeHandler::MINE, int3(0, 0, 0));
        this->mineGuards[MineTypeInfo::SAWMILL].push_back(*guardPtr);
        guardPtr = guardHandler.createGuard(GuardTypeHandler::MINE, int3(0, 0, 0));
        this->mineGuards[MineTypeInfo::ORE_PIT].push_back(*guardPtr);
    }

    for (const auto &mine : mineList.items()) {
        MineTypeInfo mineTypeInfo = getEnumFromNameOrThrow<MineTypeInfo>(mine.key());
        int count                 = mine.value().get<int>();

        if (mineTypeInfo == MineTypeInfo::RANDOM_MINE) {
            for (int i = 0; i < count; i++) {
                randomMineTypes.push_back(static_cast<MineTypeInfo>(
                    rng.nextInt(static_cast<int>(MineTypeInfo::ALCHEMISTS_LAB),
                                static_cast<int>(MineTypeInfo::SAWMILL))));
            }
        }

        this->mines[mineTypeInfo] = count;
        for (int i = 0; i < count; i++) {
            this->mineResourcesCount[mineTypeInfo].push_back(getRandomResourcesCount());

            GuardHandler guardHandler(this->rng);
            auto guardPtr = guardHandler.createGuard(GuardTypeHandler::MINE, int3(0, 0, 0));
            this->mineGuards[mineTypeInfo].push_back(*guardPtr);
        }
    }
}

void ZoneBlueprint::printZoneBlueprint() {
    cerr << "  Blueprint Zone type: " << type << "\n";
    cerr << "      Blueprint Zone richness: " << richness << "\n";
    cerr << "      Blueprint Zone placeBasicMines: " << placeBasicMines << "\n";
    cerr << "      Blueprint Zone mines:\n";
    for (const auto &mine : mines) {
        cerr << "        " << getEnumName<MineTypeInfo>(mine.first) << ": " << mine.second << "\n";
    }
}
