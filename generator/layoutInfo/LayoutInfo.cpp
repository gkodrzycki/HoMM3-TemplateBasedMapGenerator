#include "./LayoutInfo.h"


LayoutInfo::LayoutInfo() {
    name = "";
    description = "";
    mapSize = "M";
    difficulty = "";
}

void LayoutInfo::deserialize(const json& layout) {

    std::string name = getOrError<std::string>(layout, "name");
    std::string description = getOrError<std::string>(layout, "description");
    std::string mapSize = layout.value("size", "M");
    std::string difficulty = getOrError<std::string>(layout, "difficulty");

    this->name = name;
    this->description = description;
    this->mapSize = mapSize;
    this->difficulty = difficulty;

    
    // const auto& zonesConfig = getOrError<json>(config, "zones");
    // for (const auto &zoneConfig : zonesConfig)
    // {      
    //     auto zone = std::make_shared<ZoneInfo>(config.value("debug", false));
    //     zone->deserializeZone(zoneConfig, rng);
    //     zonesI[zone->getId()] = zone;
    // }

    // const auto& connectionsConfig = getOrError<json>(config, "connections");
    // for (const auto& connectionConfig : connectionsConfig) {
    //     i32 zoneA =  getOrError<int>(connectionConfig, "zoneA");
    //     i32 zoneB =  getOrError<int>(connectionConfig, "zoneB");
    //     zonesI[zoneA]->addConnection(connectionConfig);
    //     zonesI[zoneB]->addConnection(connectionConfig);
    // }
}


void LayoutInfo::printLayout() {
    std::cerr << "Template name: " << name << "\n";
    std::cerr << "Template description: " << description << "\n";
    std::cerr << "Template map size: " << mapSize << "\n";
    std::cerr << "Template difficulty: " << difficulty<< "\n";

    // for(auto zone : zonesI) {
    //     zone.second->printZone();
    // }


}