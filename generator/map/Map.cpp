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
shared_ptr<Tile> Map::getTile(int3 pos) {
    if (pos.x < 0 || pos.x >= width || pos.y < 0 || pos.y >= height)
        return nullptr;

    return tileMap[pos.y][pos.x];
}

LayoutInfo Map::getLayoutInfo() { return layoutInfo; }
RNG &Map::getRNG() { return rng; }
RegionMap Map::getRegionMap() { return regionMap; }
ZoneMap Map::getZoneMap() { return zoneMap; }
const TileMap &Map::getTileMap() { return tileMap; }
ObjectVector Map::getObjectVector() { return objectVector; }
CreatureVector Map::getCreatureVector() { return creatureVector; }

int Map::getWidth() { return width; }
int Map::getHeight() { return height; }

void Map::addRegion(shared_ptr<Region> region) { this->regionMap[region->getRegionID()] = region; }
void Map::addZone(shared_ptr<Zone> zone) { this->zoneMap[zone->getZoneID()] = zone; }
void Map::addObject(shared_ptr<Object> object) { this->objectVector.push_back(object); }
void Map::addCreature(shared_ptr<Creature> creature) { this->creatureVector.push_back(creature); }

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

    ObjectPlacer objectPlacer(*this);
    objectPlacer.placeTowns();
    objectPlacer.placeBasicMines();
    objectPlacer.placeRoads();
    objectPlacer.placeBorders();

    Creature creature = Creature("Pikeman", int3(5, 5, 0), 1, "COMPLIANT", true, true, "Creature");
    addCreature(std::make_shared<Creature>(creature));
}

void Map::printMap() {
    cerr << "==== Regions ====\n";
    for (auto &[regionID, region] : regionMap) {
        region->printRegion();
    }

    cerr << "==== Objects ====\n";
    for (auto &object : objectVector) {
        object->printObject();
    }
}
