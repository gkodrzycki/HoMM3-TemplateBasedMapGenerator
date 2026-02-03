#include "./ZoneBlueprint.hpp"

ZoneBlueprint::ZoneBlueprint() : type("support_zone"), richness("high") {}

string ZoneBlueprint::getType() { return type; }
string ZoneBlueprint::getRichness() { return richness; }

void ZoneBlueprint::deserializeZoneBlueprint(const json &zone) {
    string type     = getOrError<string>(zone, "type");
    string richness = getOrError<string>(zone, "richness");

    this->type     = type;
    this->richness = richness;
}

void ZoneBlueprint::printZoneBlueprint() {
    cerr << "      Blueprint Zone type: " << type << "\n";
    cerr << "      Blueprint Zone richness: " << richness << "\n";
}
