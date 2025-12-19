#include "./ZoneInfo.hpp"

ZoneInfo::ZoneInfo()
    : id(0), size("M"), terrain("Random"), faction("Random"), owner("None"), type("Default") {}

int ZoneInfo::getID() { return id; }
string ZoneInfo::getSize() { return size; }
string ZoneInfo::getTerrain() { return terrain; }
string ZoneInfo::getFaction() { return faction; }
string ZoneInfo::getOwner() { return owner; }
string ZoneInfo::getType() { return type; }

string ZoneInfo::getWithRegionFallback(const json &zone, const string &key,
                                       const string &regionDefault) {
    if (zone.contains(key)) {
        return zone.at(key).get<string>();
    }
    if (!regionDefault.empty()) {
        return regionDefault;
    }

    throw runtime_error("Key '" + key + "' is not defined in zone and has no region default.");
}

void ZoneInfo::deserializeZone(const json &zone, const RegionDefaults &defaults) {
    int id = getOrError<int>(zone, "id");

    string size    = getWithRegionFallback(zone, "size", defaults.size);
    string terrain = getWithRegionFallback(zone, "terrain", defaults.terrain);
    string faction = getWithRegionFallback(zone, "faction", defaults.faction);
    string owner   = getWithRegionFallback(zone, "owner_id", defaults.owner);
    string type    = getWithRegionFallback(zone, "type", defaults.type);

    this->id      = id;
    this->size    = size;
    this->terrain = terrain;
    this->faction = faction;
    this->owner   = owner;
    this->type    = type;
}

void ZoneInfo::printZone() {
    cerr << "      Zone id: " << id << "\n";
    cerr << "      Zone size: " << size << "\n";
    cerr << "      Zone terrain: " << terrain << "\n";
    cerr << "      Zone faction: " << faction << "\n";
    cerr << "      Zone owner: " << owner << "\n";
    cerr << "      Zone type: " << type << "\n";
}
