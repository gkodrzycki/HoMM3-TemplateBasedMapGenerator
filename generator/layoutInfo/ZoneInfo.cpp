#include "./ZoneInfo.hpp"

ZoneInfo::ZoneInfo() : id(0), size("M"), terrain("Random"), faction("Random"), owner("None"), type("Default") {}

int ZoneInfo::getID() {
    return id;
}
string ZoneInfo::getSize() {
    return size;
}
string ZoneInfo::getTerrain() {
    return terrain;
}
string ZoneInfo::getFaction() {
    return faction;
}
string ZoneInfo::getOwner() {
    return owner;
}
string ZoneInfo::getType() {
    return type;
}

void ZoneInfo::deserializeZone(const json& zone) {
    int id = getOrError<int>(zone, "id");

    string size = getOrError<string>(zone, "size");
    string terrain = getOrError<string>(zone, "terrain");
    string faction = getOrError<string>(zone, "faction");
    string owner = getOrError<string>(zone, "owner");
    string type = getOrError<string>(zone, "type");

    this->id = id; 
    this->terrain = terrain; 
    this->faction = faction; 
    this->owner = owner; 
    this->type = type; 
}

void ZoneInfo::printZone() {
    cerr << "      Zone id: " << id << "\n";
    cerr << "      Zone size: " << size << "\n";
    cerr << "      Zone terrain: " << terrain << "\n";
    cerr << "      Zone faction: " << faction << "\n";
    cerr << "      Zone owner: " << owner << "\n";
    cerr << "      Zone type: " << type << "\n";
}