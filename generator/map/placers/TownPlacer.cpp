#include "./TownPlacer.hpp"

TownPlacer::TownPlacer(Map &map) : map(map) {}

void TownPlacer::placeSpecificTowns(int numberOfTowns, int zoneID, bool neutral, bool upgraded) {
    auto &rng = map.getRNG();

    auto townTypes = map.getTemplateInfo().getZoneById(zoneID).getTownTypes();
    // Remove factions that are not playable in the base game (Cove, Factory, Bulwark)
    townTypes.erase(remove_if(townTypes.begin(), townTypes.end(),
                              [](const string &f) {
                                  return f == "Cove" || f == "Factory" || f == "Bulwark" ||
                                         f == "Conflux";
                              }),
                    townTypes.end());

    vector<int3> freeTiles;
    for (int x = 0; x < map.getWidth(); x++) {
        for (int y = 0; y < map.getHeight(); y++) {
            auto tile = map.getTile(int3(x, y, 0));
            if (tile->getZoneID() == zoneID && tile->getTileType() == TileType::TILE_FREE &&
                x > 2 && y > 2 && x < map.getWidth() - 3 && y < map.getHeight() - 3) {
                freeTiles.push_back(int3(x, y, 0));
            }
        }
    }

    auto playerTownsSettings = map.getTemplateInfo().getZoneById(zoneID).getPlayerTowns();

    int3 offset(1, 1, 0);
    int ownership   = neutral ? -1 : playerTownsSettings.ownership;
    string townName = neutral ? "Neutral Town" : "Player Town";

    for (int i = 0; i < numberOfTowns; i++) {
        auto townType     = rng.getRandomFromVector(townTypes);
        auto townTypeEnum = getEnumFromNameOrThrow<Faction>(townType);

        auto townPos = rng.getRandomFromVector(freeTiles);

        Town town(townTypeEnum, ownership, townPos + offset, townName);
        auto townPtr = make_shared<Town>(town);
        map.addObject(townPtr);

        map.fixNeighbourTiles(townPtr->getPosition(), townPtr->getSize(), zoneID, offset);
        int3 entrancePos = townPtr->getPosition() - int3(townPtr->getSize().x / 2, 0, 0);
        map.getTile(entrancePos)->setTileType(TileType::TILE_OCCUPIED);
    }
}

void TownPlacer::placeTowns() {
    ZoneMap zoneMap = map.getZoneMap();

    for (auto [zoneID, zone] : zoneMap) {
        string zoneType = zone->getType();

        auto playerTownsSettings  = map.getTemplateInfo().getZoneById(zoneID).getPlayerTowns();
        auto neutralTownsSettings = map.getTemplateInfo().getZoneById(zoneID).getNeutralTowns();

        placeSpecificTowns(playerTownsSettings.minimumCastles, zoneID, false, false);
        placeSpecificTowns(neutralTownsSettings.minimumTowns, zoneID, true, false);
    }
}
