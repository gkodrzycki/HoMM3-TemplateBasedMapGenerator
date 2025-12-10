#include "./RegionPlacer.hpp"

RegionPlacer::RegionPlacer(Map &map) : map(map) {}

void RegionPlacer::generateRegions() {

    LayoutInfo layout = map.getLayoutInfo();

    for (auto regionInfo : layout.getRegionInfoList()) {

        Region region(regionInfo);
        for (auto zoneInfo : regionInfo.getZoneInfoList()) {
            Zone zone(zoneInfo);

            zone.setCenter(int3(10 * zone.getZoneID(), 10 * zone.getZoneID(),
                                0)); // TODO change to something more clever
            region.addZone(make_shared<Zone>(zone));
        }

        map.addRegion(make_shared<Region>(region));
    }
}