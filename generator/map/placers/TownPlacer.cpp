#include "./TownPlacer.hpp"

TownPlacer::TownPlacer(Map &map) : map(map) {}

void TownPlacer::placeSpecificTowns(TownSettings townSettings, vector<string> &townTypes,
                                    int zoneID, bool neutral, bool upgraded,
                                    vector<int3> &freeTiles, vector<int3> &placedTowns) {
    int3 offset(1, 1, 0);
    auto &rng       = map.getRNG();
    int ownership   = neutral ? -1 : townSettings.ownership;
    string townName = neutral ? "Neutral Town" : "Player Town";

    int numberOfTowns = upgraded ? townSettings.minimumCastles : townSettings.minimumTowns;

    int3 zoneCenter = map.getZoneMap()[zoneID]->getCenter();
    string zoneType = map.getZoneMap()[zoneID]->getType();

    if (zoneType == "treasure") {
        // For treasure zones treat each border as placed object so every player will have "similar"
        // distance
        for (int x = 0; x < map.getWidth(); x++) {
            for (int y = 0; y < map.getHeight(); y++) {
                int3 tilePos = int3(x, y, 0);
                auto tile    = map.getTile(tilePos);
                if (tile->getZoneID() == zoneID && tile->isTileType("Bb")) {
                    bool onBorder = false;
                    for (int dx = -1; dx <= 1; dx++) {
                        for (int dy = -1; dy <= 1; dy++) {
                            int3 neighboursPos = tilePos + int3(dx, dy, 0);
                            auto neighbourTile = map.getTile(neighboursPos);
                            if (neighbourTile && neighbourTile->getZoneID() != zoneID) {
                                onBorder = true;
                                break;
                            }
                        }
                    }
                    if (onBorder) {
                        placedTowns.push_back(tilePos);
                    }
                }
            }
        }
    }

    for (int i = 0; i < numberOfTowns; i++) {

        vector<int3> currentFreeTiles;
        for (const auto &pos : freeTiles) {
            if (map.getTile(pos)->getTileType() == TileType::TILE_FREE) {
                currentFreeTiles.push_back(pos);
            }
        }

        if (currentFreeTiles.empty())
            break;

        float tolerance = (zoneType == "treasure") ? 0.6f : 0.8f;
        int3 townPos    = map.findBestDistributedPosition(currentFreeTiles, placedTowns, zoneCenter,
                                                          rng, tolerance);

        if (townPos.x == -1)
            break;

        auto townType     = rng.getRandomFromVector(townTypes);
        auto townTypeEnum = neutral ? getEnumFromNameOrThrow<Faction>(townType)
                                    : map.getZoneMap()[zoneID]->getFaction();

        Town town(townTypeEnum, ownership, upgraded, townPos + offset, townName);
        auto townPtr = make_shared<Town>(town);
        map.addObject(townPtr);

        map.fixNeighbourTiles(townPtr->getPosition(), townPtr->getSize(), zoneID, offset);
        int3 entrancePos = townPtr->getPosition() - int3(townPtr->getSize().x / 2, 0, 0);
        map.getTile(entrancePos)->setTileType(TileType::TILE_OCCUPIED);

        placedTowns.push_back(townPos);
    }
}

void TownPlacer::placeTowns() {
    ZoneMap zoneMap = map.getZoneMap();
    int mapWidth    = map.getWidth();
    int mapHeight   = map.getHeight();

    for (auto [zoneID, zone] : zoneMap) {
        string zoneType = zone->getType();

        auto townTypes = map.getTemplateInfo().getZoneById(zoneID).getTownTypes();
        // Remove factions that are not playable in the base game (Cove, Factory, Bulwark, Conflux)
        townTypes.erase(remove_if(townTypes.begin(), townTypes.end(),
                                  [](const string &f) {
                                      return f == "Cove" || f == "Factory" || f == "Bulwark" ||
                                             f == "Conflux";
                                  }),
                        townTypes.end());

        auto playerTownsSettings  = map.getTemplateInfo().getZoneById(zoneID).getPlayerTowns();
        auto neutralTownsSettings = map.getTemplateInfo().getZoneById(zoneID).getNeutralTowns();

        vector<int3> freeTiles;

        for (int x = 0; x < mapWidth; x++) {
            for (int y = 0; y < mapHeight; y++) {
                int3 pos(x, y, 0);
                auto tile = map.getTile(pos);
                if (tile->getZoneID() == zoneID && tile->getTileType() == TileType::TILE_FREE) {
                    if (!isInside(2, 2, mapWidth - 2, mapHeight - 2, pos) ||
                        map.checkPlacementConflict(pos, int3(5, 3, 0), "BbTRr", int3(1, 1, 0))) {
                        continue;
                    }
                    freeTiles.push_back(pos);
                }
            }
        }

        vector<int3> placedTowns;
        // Player Castles
        placeSpecificTowns(playerTownsSettings, townTypes, zoneID, false, true, freeTiles,
                           placedTowns);
        // Player Towns
        placeSpecificTowns(playerTownsSettings, townTypes, zoneID, false, false, freeTiles,
                           placedTowns);
        // Neutral Castles
        placeSpecificTowns(neutralTownsSettings, townTypes, zoneID, true, true, freeTiles,
                           placedTowns);
        // Neutral Towns
        placeSpecificTowns(neutralTownsSettings, townTypes, zoneID, true, false, freeTiles,
                           placedTowns);

        // if(placedTowns.size() > 0){
        //     cerr << "Changing zone center for zone " << zoneID << " from " <<
        //     map.getZoneMap()[zoneID]->getCenter().toString() << " to " << (placedTowns[0] +
        //     int3(1, 1, 0)).toString() << endl;
        //     // map.getZoneMap()[zoneID]->setCenter(placedTowns[0] + int3(1, 1, 0)); // TODO:
        //     improve center calculation
        // }
    }
}
