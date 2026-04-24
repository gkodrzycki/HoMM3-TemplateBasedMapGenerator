#include "./TownPlacer.hpp"

TownPlacer::TownPlacer(Map &map) : map(map) {}

void TownPlacer::placeTowns() {
    ZoneMap zoneMap = map.getZoneMap();
    int3 offset(1, 1, 0);

    for (auto [zoneID, zone] : zoneMap) {
        string zoneType = zone->getType();

        // TODO place neutral and player towns as described in template, currently just placing one
        // town in each start zone
        if (zoneType == "human_start" || zoneType == "computer_start") {
            Town town(zone->getFaction(), zone->getOwner(), zone->getCenter() + int3(2, 0, 0),
                      "Town"); // offset from center
            auto townPtr = make_shared<Town>(town);
            map.addObject(townPtr);

            map.fixNeighbourTiles(townPtr->getPosition(), townPtr->getSize(), zoneID, offset);
            int3 entrancePos = townPtr->getPosition() - int3(townPtr->getSize().x / 2, 0, 0);
            map.getTile(entrancePos)->setTileType(TileType::TILE_OCCUPIED);
        }
    }
}
