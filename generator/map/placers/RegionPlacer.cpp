#include "./RegionPlacer.hpp"

RegionPlacer::RegionPlacer(Map &map) : map(map) {}

void RegionPlacer::generateRegions() {

    LayoutInfo layout = map.getLayoutInfo();

    for (auto regionInfo : layout.getRegionInfoList()) {

        Region region(regionInfo);
        for (auto zoneInfo : regionInfo.getZoneInfoList()) {
            Zone zone(zoneInfo);

            if (zone.getZoneID() == 1) {
                zone.setCenter(int3(5, 5, 0));
            } else if (zone.getZoneID() == 2) {
                zone.setCenter(int3(30, 5, 0));
            } else if (zone.getZoneID() == 3) {
                zone.setCenter(int3(5, 30, 0));
            } else if (zone.getZoneID() == 4) {
                zone.setCenter(int3(30, 30, 0));
            } // TODO change to something more clever

            auto zonePtr = make_shared<Zone>(zone);
            region.addZone(zonePtr);
            map.addZone(zonePtr);
        }

        map.addRegion(make_shared<Region>(region));
    }
}

void RegionPlacer::claimTiles(vector<pair<int, int3>> &zoneCenters) {
    int mapWidth = map.getWidth(), mapHeight = map.getHeight();

    auto neighbors4 = [&](const int3 &p) {
        std::array<int3, 4> out;
        for (int i = 0; i < 4; i++)
            out[i] = p + directions4[i];
        return out;
    };

    auto passable = [&](const int3 &p) { return true; };

    auto claim = bfs_claim_xyz2(mapWidth, mapHeight, zoneCenters, neighbors4, passable);

    ZoneMap zoneMap = map.getZoneMap();

    bool debug = (map.getLayoutInfo().getDebug() > 0);
    if (debug)
        cerr << "==== ZoneIDs on Map ====\n";

    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            int zoneID = claim[x][y][0];
            if (debug) {
                bool isZoneCenter = false;
                for (auto &[zID, zone] : zoneMap) {
                    if (zone->getCenter() == int3(x, y, 0)) {
                        isZoneCenter = true;
                        break;
                    }
                }

                if (isZoneCenter) {
                    printColor(BRIGHT_RED + BOLD, '*');
                } else {
                    printColor(getZoneColor(zoneID), to_string(zoneID));
                }
            }

            auto tilePtr = map.getTile(int3(x, y, 0));
            tilePtr->setZoneID(zoneID);
            tilePtr->setTerrain(zoneMap[zoneID]->getTerrain());
        }
        if (debug)
            cerr << "\n";
    }
}

void RegionPlacer::placeRegions() {

    RegionMap regionMap = map.getRegionMap();

    vector<pair<int, int3>> zoneCenters;

    for (auto [regionID, region] : regionMap) {
        ZoneMap zoneMap = region->getZoneMap();
        for (auto [zoneID, zone] : zoneMap) {
            zoneCenters.push_back({zoneID, zone->getCenter()});
        }
    }
    claimTiles(zoneCenters);
}
