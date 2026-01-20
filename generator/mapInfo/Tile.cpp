#include "./Tile.hpp"

Tile::Tile() : tileType(TileType::TILE_FREE) {}
Tile::Tile(int zoneID, string terrain)
    : zoneID(zoneID), terrain(terrain), tileType(TileType::TILE_FREE) {}
Tile::Tile(int zoneID, string terrain, TileType tileType)
    : zoneID(zoneID), terrain(terrain), tileType(tileType) {}

void Tile::setZoneID(int zoneID) { this->zoneID = zoneID; }
void Tile::setTerrain(string terrain) { this->terrain = terrain; }
void Tile::setTileType(TileType tileType) { this->tileType = tileType; }

int Tile::getZoneID() { return zoneID; }
string Tile::getTerrain() { return terrain; }
TileType Tile::getTileType() { return tileType; }

bool Tile::isTileType(string types) {
    for (auto type : types) {
        if (tileType == charToTileType(type))
            return true;
    }
    return false;
}

void Tile::printTile() {
    cerr << "    Tile Zone ID: " << zoneID << "\n";
    cerr << "    Tile Terrain: " << terrain << "\n";
    cerr << "    Tile Type: " << getEnumName<TileType>(tileType) << "\n";
}

char tileTypeToChar(TileType tileType) {
    switch (tileType) {
    case TileType::TILE_FREE:
        return 'F';
    case TileType::TILE_TAKEN:
        return 'T';
    case TileType::TILE_OCCUPIED:
        return 'O';
    case TileType::TILE_ROAD:
        return 'r';
    case TileType::TILE_RESERVED:
        return 'R';
    case TileType::TILE_BORDER_INNER:
        return 'B';
    case TileType::TILE_BORDER_OUTER:
        return 'b';
    default:
        return '?';
    }
}
TileType charToTileType(char c) {
    switch (c) {
    case 'F':
        return TileType::TILE_FREE;
    case 'T':
        return TileType::TILE_TAKEN;
    case 'O':
        return TileType::TILE_OCCUPIED;
    case 'r':
        return TileType::TILE_ROAD;
    case 'R':
        return TileType::TILE_RESERVED;
    case 'B':
        return TileType::TILE_BORDER_INNER;
    case 'b':
        return TileType::TILE_BORDER_OUTER;
    default:
        return TileType::TILE_FREE;
    }
}
