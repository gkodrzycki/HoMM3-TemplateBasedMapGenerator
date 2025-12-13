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

    int width = map.getWidth();
    int height = map.getHeight();
    int currentClaim[height][width][2];

    fill(&currentClaim[0][0][0], &currentClaim[0][0][0] + height * width * 2, 0);

    queue<int3> q;
    for (auto [zoneID, zoneCenter] : zoneCenters) {
        currentClaim[zoneCenter.y][zoneCenter.x][zoneCenter.z] = zoneID;
        q.push(zoneCenter);
    }

    int3 directions[] = {
        // TODO think about 8 directions
        int3(1, 0, 0),
        int3(0, 1, 0),
        int3(-1, 0, 0),
        int3(0, -1, 0),
        // int3(1,1,0),
        // int3(-1,-1,0),
        // int3(-1,1,0),
        // int3(1,-1,0),
    };

    while (q.size()) {
        int3 currentPos = q.front();
        q.pop();
        int currentID = currentClaim[currentPos.y][currentPos.x][currentPos.z];

        for (auto direction : directions) {
            int3 newPos = currentPos + direction;
            if (newPos.x < 0 || newPos.y < 0 || newPos.x >= width || newPos.y >= height)
                continue;
            if (currentClaim[newPos.y][newPos.x][newPos.z] != 0)
                continue;
            currentClaim[newPos.y][newPos.x][newPos.z] = currentID;
            q.push(newPos);
        }
    }

    ZoneMap zoneMap = map.getZoneMap();

    cerr << "==== ZoneIDs on Map ====\n";
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int zoneID = currentClaim[y][x][0];
            cerr << zoneID << " ";
            auto tilePtr = map.getTile(int3(x, y, 0));
            tilePtr->setZoneID(zoneID);
            tilePtr->setTerrain(zoneMap[zoneID]->getTerrain());
        }
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
