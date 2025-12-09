#include "./ZoneInfo.h"

ZoneInfo::ZoneInfo() : id(0), size("M"), terrain("Random"), faction("Random"), owner("None"), type("Default") {}

void ZoneInfo::deserializeZone(const json& zone) {
    int id = getOrError<int>(zone, "id");

    std::string size = getOrError<std::string>(zone, "size");
    std::string terrain = getOrError<std::string>(zone, "terrain");
    std::string faction = getOrError<std::string>(zone, "faction");
    std::string owner = getOrError<std::string>(zone, "owner");
    std::string type = getOrError<std::string>(zone, "type");

    this->id = id; 
    this->terrain = terrain; 
    this->faction = faction; 
    this->owner = owner; 
    this->type = type; 
}

void ZoneInfo::printZone() {
    std::cerr << "      Zone id: " << id << "\n";
    std::cerr << "      Zone size: " << size << "\n";
    std::cerr << "      Zone terrain: " << terrain << "\n";
    std::cerr << "      Zone faction: " << faction << "\n";
    std::cerr << "      Zone owner: " << owner << "\n";
    std::cerr << "      Zone type: " << type << "\n";
}