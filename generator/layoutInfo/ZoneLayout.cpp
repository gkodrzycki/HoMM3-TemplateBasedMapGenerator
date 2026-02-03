#include "./ZoneLayout.hpp"

ZoneLayout::ZoneLayout()
    : id(0), size("M"), terrain("Random"), faction("Random"), owner("None"), type("Default") {}

int ZoneLayout::getID() { return id; }
string ZoneLayout::getSize() { return size; }
string ZoneLayout::getTerrain() { return terrain; }
string ZoneLayout::getFaction() { return faction; }
string ZoneLayout::getOwner() { return owner; }
string ZoneLayout::getType() { return type; }

string ZoneLayout::getWithRegionFallback(const json &zone, const string &key,
                                         const string &regionDefault) {
    if (zone.contains(key)) {
        return zone.at(key).get<string>();
    }
    if (!regionDefault.empty()) {
        return regionDefault;
    }

    throw runtime_error("Key '" + key + "' is not defined in zone and has no region default.");
}

void ZoneLayout::deserializeZone(const json &zone, const RegionDefaults &defaults) {
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

void ZoneLayout::printZone() {
    cerr << "      Zone id: " << id << "\n";
    cerr << "      Zone size: " << size << "\n";
    cerr << "      Zone terrain: " << terrain << "\n";
    cerr << "      Zone faction: " << faction << "\n";
    cerr << "      Zone owner: " << owner << "\n";
    cerr << "      Zone type: " << type << "\n";
}
