#include "./ZoneBlueprint.hpp"

ZoneBlueprint::ZoneBlueprint() : type("support_zone"), richness("high") {}

string ZoneBlueprint::getType() { return type; }
string ZoneBlueprint::getRichness() { return richness; }
bool ZoneBlueprint::getPlaceBasicMines() { return placeBasicMines; }
map<MineTypeInfo, int> &ZoneBlueprint::getMines() { return mines; }

void ZoneBlueprint::deserializeZoneBlueprint(const json &zone) {
    string type          = getOrError<string>(zone, "type");
    string richness      = getOrError<string>(zone, "richness");
    bool placeBasicMines = getOrDefault<bool>(zone, "placeBasicMines", true);

    const auto &mineList = getOrDefault<json>(zone, "mines", json::object());

    for (const auto &mine : mineList.items()) {
        MineTypeInfo mineTypeInfo = getEnumFromNameOrThrow<MineTypeInfo>(mine.key());
        int count                 = mine.value().get<int>();
        this->mines[mineTypeInfo] = count;
    }

    this->type            = type;
    this->richness        = richness;
    this->placeBasicMines = placeBasicMines;
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
