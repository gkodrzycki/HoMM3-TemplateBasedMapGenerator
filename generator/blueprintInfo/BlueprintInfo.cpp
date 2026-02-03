#include "./BlueprintInfo.hpp"

BlueprintInfo::BlueprintInfo() {}

vector<ZoneBlueprint> BlueprintInfo::getBlueprints() { return blueprints; }

ZoneBlueprint BlueprintInfo::getTypeBlueprint(const string &type) {
    for (auto blueprint : blueprints) {
        if (blueprint.getType() == type) {
            return blueprint;
        }
    }
    throw out_of_range("Blueprint not found: " + type);
}

void BlueprintInfo::deserialize(const json &blueprint) {
    const auto &zonesList = getOrError<json>(blueprint, "zones");
    for (const auto &zone : zonesList) {
        ZoneBlueprint zoneBlueprint;
        zoneBlueprint.deserializeZoneBlueprint(zone);
        blueprints.push_back(zoneBlueprint);
    }
}

void BlueprintInfo::printBlueprint() {
    for (auto blueprint : blueprints) {
        blueprint.printZoneBlueprint();
    }
}
