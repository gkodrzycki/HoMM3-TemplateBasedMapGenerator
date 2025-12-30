#include "./Tile.hpp"

Tile::Tile() : road(nullptr) {}

Tile::Tile(int zoneID, string terrain) : zoneID(zoneID), terrain(terrain), road(nullptr) {}

void Tile::setZoneID(int zoneID) { this->zoneID = zoneID; }
void Tile::setTerrain(string terrain) { this->terrain = terrain; }
void Tile::setRoad(shared_ptr<Road> road) { this->road = road; }

int Tile::getZoneID() { return zoneID; }
string Tile::getTerrain() { return terrain; }
shared_ptr<Road> Tile::getRoad() { return road; }

void Tile::printTile() {
    cerr << "    Tile Zone ID: " << zoneID << "\n";
    cerr << "    Tile Terrain: " << terrain << "\n";
}
