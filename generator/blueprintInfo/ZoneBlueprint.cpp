#include "./ZoneBlueprint.hpp"

ZoneBlueprint::ZoneBlueprint() : type("support_zone"), richness("high") {}

string ZoneBlueprint::getType() { return type; }
string ZoneBlueprint::getRichness() { return richness; }
bool ZoneBlueprint::getPlaceBasicMines() { return placeBasicMines; }
map<string, int> &ZoneBlueprint::getMines() { return mines; }

void ZoneBlueprint::deserializeZoneBlueprint(const json &zone) {
    string type          = getOrError<string>(zone, "type");
    string richness      = getOrError<string>(zone, "richness");
    bool placeBasicMines = getOrDefault<bool>(zone, "placeBasicMines", true);

    const auto &mineList = getOrDefault<json>(zone, "mines", json::object());

    for (const auto &mine : mineList.items()) {
        string mineType       = mine.key();
        int count             = mine.value().get<int>();
        this->mines[mineType] = count;
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
        cerr << "        " << mine.first << ": " << mine.second << "\n";
    }
}
