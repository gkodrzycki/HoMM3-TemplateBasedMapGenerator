#include "./ObjectPlacer.hpp"

ObjectPlacer::ObjectPlacer(Map &map) : map(map) {}

void ObjectPlacer::placeTowns() {
    ZoneMap zoneMap = map.getZoneMap();

    for (auto [zoneID, zone] : zoneMap) {
        string zoneType = zone->getType();

        if (zoneType == "starting_zone") {
            cerr << "Placing town " << factionToString(zone->getFaction()) << " at "
                 << zone->getCenter().toString() << "\n";

            Town town(zone->getFaction(), zone->getOwner(), zone->getCenter(), "Town");
            auto townPtr = make_shared<Town>(town);
            map.addObject(townPtr);
        }
    }
}
