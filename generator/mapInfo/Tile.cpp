#include "./Tile.hpp"

Tile::Tile() : tileType(TileType::TILE_FREE), road(nullptr) {}
Tile::Tile(int zoneID, string terrain)
    : zoneID(zoneID), terrain(terrain), tileType(TileType::TILE_FREE), road(nullptr) {}
Tile::Tile(int zoneID, string terrain, TileType tileType)
    : zoneID(zoneID), terrain(terrain), tileType(tileType), road(nullptr) {}

void Tile::setZoneID(int zoneID) { this->zoneID = zoneID; }
void Tile::setTerrain(string terrain) { this->terrain = terrain; }
void Tile::setRoad(shared_ptr<Road> road) { this->road = road; }
void Tile::setTileType(TileType tileType) { this->tileType = tileType; }

int Tile::getZoneID() { return zoneID; }
string Tile::getTerrain() { return terrain; }
shared_ptr<Road> Tile::getRoad() { return road; }
TileType Tile::getTileType() { return tileType; }

void Tile::printTile() {
    cerr << "    Tile Zone ID: " << zoneID << "\n";
    cerr << "    Tile Terrain: " << terrain << "\n";
    cerr << "    Tile Type: " << tileTypeToString(tileType) << "\n";
}

string Tile::tileTypeToString(TileType tileType) {
    switch (tileType) {
    case TileType::TILE_FREE:
        return "TILE_FREE";
    case TileType::TILE_TAKEN:
        return "TILE_TAKEN";
    default:
        return "UNKNOWN_TILE_TYPE";
    }
}
TileType Tile::stringToTileType(string tileType) {
    if (tileType == "TILE_FREE")
        return TileType::TILE_FREE;
    if (tileType == "TILE_TAKEN")
        return TileType::TILE_TAKEN;
    return TileType::TILE_FREE;
}
