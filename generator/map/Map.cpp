#include "./Map.hpp"

Map::Map(RNG &rng, LayoutInfo layoutInfo, BlueprintInfo blueprintInfo)
    : rng(rng), layoutInfo(layoutInfo), blueprintInfo(blueprintInfo) {}

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
BlueprintInfo Map::getBlueprintInfo() { return blueprintInfo; }
RNG &Map::getRNG() { return rng; }
RegionMap Map::getRegionMap() { return regionMap; }
ZoneMap Map::getZoneMap() { return zoneMap; }
const TileMap &Map::getTileMap() { return tileMap; }
ObjectVector Map::getObjectVector() { return objectVector; }
RoadVector Map::getRoadVector() { return roadVector; }
CreatureVector Map::getCreatureVector() { return creatureVector; }
TreasureVector Map::getTreasureVector() { return treasureVector; }
array<int, 4> &Map::getBasicResourceCount() { return basicResourceCount; }

int Map::getWidth() { return width; }
int Map::getHeight() { return height; }

void Map::addRegion(shared_ptr<Region> region) { this->regionMap[region->getRegionID()] = region; }
void Map::addZone(shared_ptr<Zone> zone) { this->zoneMap[zone->getZoneID()] = zone; }
void Map::addObject(shared_ptr<Object> object) { this->objectVector.push_back(object); }
void Map::addRoad(shared_ptr<Road> road) { this->roadVector.push_back(road); }
void Map::addCreature(shared_ptr<Creature> creature) { this->creatureVector.push_back(creature); }
void Map::addTreasure(shared_ptr<Treasure> treasure) { this->treasureVector.push_back(treasure); }
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

void Map::initMap() {
    initTiles();

    // TODO Choose values based on layout/blueprint info richness?
    for (int i = 0; i < 4; i++) {
        basicResourceCount[i] = rng.nextInt(0, 15);
    }
}

void Map::generateMap() {
    if (layoutInfo.getDebug() > 0) {
        cerr << "==== SEED: " << rng.getOriginalSeed() << " ====\n";
    }

    initMap();

    RegionPlacer regionPlacer(*this);
    regionPlacer.placeRegions();

    BorderPlacer borderPlacer(*this);
    borderPlacer.reserveBorderTiles();

    TerrainPlacer terrainPlacer(*this);
    terrainPlacer.generateNoise();

    TownPlacer townPlacer(*this);
    townPlacer.placeTowns();

    RoadPlacer roadPlacer(*this);
    roadPlacer.placeRoads();

    ObjectPlacer objectPlacer(*this);
    // objectPlacer.placeBasicMines();
    // objectPlacer.placeMines();
    // objectPlacer.placeMineResources();
    // objectPlacer.placeTreasures();

    GuardPlacer guardPlacer(*this);
    // guardPlacer.placeGuards();

    // borderPlacer.placeBorders();
    // terrainPlacer.placeObstacles();
}

void Map::fixNeighbourTiles(const int3 &pos, const int3 &size, int zoneID, const int3 &offset) {
    auto zoneMap       = getZoneMap();
    string zoneTerrain = zoneMap[zoneID]->getTerrain();

    for (int dx = 0; dx < size.x + offset.x; dx++) {
        for (int dy = 0; dy < size.y + offset.y; dy++) {
            int3 tilePos(pos.x - size.x + dx + 1, pos.y - size.y + dy + 1, pos.z);
            auto tilePtr = getTile(tilePos);

            if (tilePtr == nullptr)
                continue;

            if (offset.x >= 1 && offset.y >= 1 &&
                (dx == size.x + offset.x - 1 || dy == size.y + offset.y - 1)) {
                tilePtr->setTileType(TileType::TILE_RESERVED);
            } else {
                tilePtr->setTileType(TileType::TILE_TAKEN);
            }
            tilePtr->setZoneID(zoneID);
            tilePtr->setTerrain(zoneTerrain);
        }
    }
}

bool Map::checkPlacementConflict(const int3 &pos, const int3 &size, const string &types,
                                 const int3 &offset, bool debug) {
    auto zoneMap = getZoneMap();

    if (debug) {
        cerr << "debugging checkplacement conflict\n";
    }

    for (int dx = 0; dx < size.x + offset.x; dx++) {
        for (int dy = 0; dy < size.y + offset.y; dy++) {
            int3 tilePos(pos.x - size.x + dx + 1, pos.y - size.y + dy + 1, pos.z);
            auto tilePtr = getTile(tilePos);

            if (tilePtr == nullptr)
                continue;

            if (debug) {
                cerr << tilePos.toString() << "\n";
            }

            if (tilePtr->isTileType(types)) {
                return true;
            }
        }
    }
    return false;
}

void Map::printMap(int debugLevel) {
    if (debugLevel > 1) {
        cerr << "==== Regions ====\n";
        for (auto &[regionID, region] : regionMap) {
            region->printRegion();
        }
    }

    if (debugLevel > 2) {
        cerr << "==== Objects ====\n";
        for (auto &object : objectVector) {
            object->printObject();
        }
    }

    cerr << "==== Tiles ====\n";
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            TileType tileType = tileMap[i][j]->getTileType();
            switch (tileType) {
            case TileType::TILE_DEBUG:
                printColor(RED, tileTypeToChar(tileType));
                break;
            case TileType::TILE_FREE:
                printColor(GREEN, tileTypeToChar(tileType));
                break;
            case TileType::TILE_OCCUPIED:
                printColor(YELLOW, tileTypeToChar(tileType));
                break;
            case TileType::TILE_RESERVED:
                cerr << tileTypeToChar(tileType);
                break;
            case TileType::TILE_TAKEN:
                printColor(CYAN, tileTypeToChar(tileType));
                break;
            case TileType::TILE_ROAD:
                printColor(MAGENTA, tileTypeToChar(tileType));
                break;
            case TileType::TILE_BORDER_INNER:
                printColor(BLUE, tileTypeToChar(tileType));
                break;
            case TileType::TILE_BORDER_OUTER:
                printColor(BRIGHT_BLUE, tileTypeToChar(tileType));
                break;
            default:
                cerr << tileTypeToChar(tileType);
            }
        }
        cerr << "\n";
    }
}
