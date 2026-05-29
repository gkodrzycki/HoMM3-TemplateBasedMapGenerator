#include "TreasureInfo.hpp"

MapObjectDefinition MapObjectRegistry::getRandomObject(ObjectCategory category, int targetValue,
                                                       int tolerance, RNG &rng) {
    std::vector<MapObjectDefinition> validCandidates;

    for (const auto &obj : allObjects) {
        if (obj.category == category) {
            // Sprawdzamy czy wartość mieści się w widełkach (np. +/- 20%)
            if (obj.value >= targetValue - tolerance && obj.value <= targetValue + tolerance) {
                validCandidates.push_back(obj);
            }
        }
    }

    if (validCandidates.empty()) {
        throw std::runtime_error("No objects found for category and value criteria.");
    }

    // Losujemy jeden z pasujących obiektów
    int index = rng.nextInt(0, validCandidates.size() - 1);
    return validCandidates[index];
}

int MapObjectRegistry::getObjectValue(const std::string &objectName) {
    auto it = objectByName.find(objectName);
    if (it != objectByName.end()) {
        return it->second.value;
    }
    return 0;
}

ObjectCategory determineCategory(const std::string &objectName) {
    if (objectName == "cyclopsStockpile" || objectName == "dwarvenTreasury" ||
        objectName == "griffinConservatory" || objectName == "impCache" ||
        objectName == "medusaStore" || objectName == "nagaBank" || objectName == "dragonFlyHive" ||
        objectName == "crypt" || objectName == "dragonUtopia") {
        return ObjectCategory::CREATURE_BANK;
    } else if (objectName == "Ore" || objectName == "Gold" || objectName == "Gems" ||
               objectName == "Crystal" || objectName == "Mercury" || objectName == "Sulfur" ||
               objectName == "Wood") {
        return ObjectCategory::RESOURCE;
    } else if (objectName == "randomArtifactTreasure" || objectName == "randomArtifactMinor" ||
               objectName == "randomArtifactMajor" || objectName == "randomArtifactRelic" ||
               objectName == "campfire" || objectName == "treasureChest") {
        return ObjectCategory::PICKUP;
    } else if (objectName == "pandoraBox") {
        return ObjectCategory::PANDORAS_BOX;
    } else {
        return ObjectCategory::VISITABLE;
    }
}

void MapObjectRegistry::loadFromJson(const std::string &filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filepath);
    }
    json jsonData;
    file >> jsonData;

    for (const auto &item : jsonData) {
        MapObjectDefinition obj;
        obj.objectName = item["objectName"].get<std::string>();
        obj.value      = item["value"].get<int>();
        obj.category   = determineCategory(obj.objectName);

        allObjects.push_back(obj);
        objectByName[obj.objectName] = obj;
    }
}
