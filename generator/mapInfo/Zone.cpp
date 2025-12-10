#include "./Zone.hpp"

Zone::Zone() : zoneID(0), size("M"), terrain("Random"), faction("Random"), owner("None"), type("Default") {}

void Zone::setCenter(int3 center) {
    this->center = center;
}

void Zone::setZoneID(int zoneID) {
    this->zoneID = zoneID;
}

void Zone::setSize(string size) {
    this->size = size;
}

void Zone::setTerrain(string terrain) {
    this->terrain = terrain;
}

void Zone::setFaction(string faction) {
    this->faction = faction;
}

void Zone::setOwner(string owner) {
    this->owner = owner;
}

void Zone::setType(string type) {
    this->type = type;
}

int3 Zone::getCenter() {
    return center;
}

int Zone::getZoneID() {
    return zoneID;
}

string Zone::getSize() {
    return size;
}

string Zone::getTerrain() {
    return terrain;
}

string Zone::getFaction() {
    return faction;
}

string Zone::getOwner() {
    return owner;
}

string Zone::getType() {
    return type;
}

