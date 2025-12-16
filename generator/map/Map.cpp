#include "./Map.hpp"

Map::Map(RNG &rng, LayoutInfo layoutInfo) : rng(rng), layoutInfo(layoutInfo) {}

pair<int, int> decodeMapSize(string mapSize) {
    if (mapSize == "S")
        return {36, 36};
    if (mapSize == "M")
        return {72, 72};
    if (mapSize == "L")
        return {108, 108};
    if (mapSize == "XL")
        return {144, 144};
    return {-1, -1};
}
shared_ptr<Tile> Map::getTile(int3 pos) { return tileMap[pos.y][pos.x]; }
LayoutInfo Map::getLayoutInfo() { return layoutInfo; }
RNG &Map::getRNG() { return rng; }
RegionMap Map::getRegionMap() { return regionMap; }
ZoneMap Map::getZoneMap() { return zoneMap; }
int Map::getWidth() { return width; }
int Map::getHeight() { return height; }

void Map::addRegion(shared_ptr<Region> region) { this->regionMap[region->getRegionID()] = region; }
void Map::addZone(shared_ptr<Zone> zone) { this->zoneMap[zone->getZoneID()] = zone; }

void Map::initTiles() {
    pair<int, int> width_height = decodeMapSize(layoutInfo.getMapSize());

    width  = width_height.first;
    height = width_height.second;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            tileMap[i][j] = std::make_shared<Tile>();
        }
    }
}

void Map::generateMap() {

    initTiles();

    RegionPlacer regionPlacer(*this);
    regionPlacer.generateRegions();
    regionPlacer.placeRegions();
}

void Map::printMap() {
    cerr << "==== Regions ====\n";
    for (auto &[regionID, region] : regionMap) {
        region->printRegion();
    }
}
