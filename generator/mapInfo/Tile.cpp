#include "./Tile.hpp"

Tile::Tile() {}

Tile::Tile(int zoneID, string terrain) : zoneID(zoneID), terrain(terrain) {}

void Tile::setZoneID(int zoneID) { this->zoneID = zoneID; }
void Tile::setTerrain(string terrain) { this->terrain = terrain; }

int Tile::getZoneID() { return zoneID; }

string Tile::getTerrain() { return terrain; }