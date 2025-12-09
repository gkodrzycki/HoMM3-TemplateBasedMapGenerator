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

    
    const auto& regionList = getOrError<json>(layout, "regions");
    for (const auto& region : regionList) {
        RegionInfo regionInfo;
        regionInfo.deserializeRegion(region);
        regionInfoList.push_back(regionInfo);
    }

    const auto& connectionList = getOrError<json>(layout, "connections");
    for (const auto& connection : connectionList) {
        ConnectionInfo connectionInfo;
        connectionInfo.deserializeConnection(connection);
        connectionInfoList.push_back(connectionInfo);
    }
}


void LayoutInfo::printLayout() {
    std::cerr << "Template name: " << name << "\n";
    std::cerr << "Template description: " << description << "\n";
    std::cerr << "Template map size: " << mapSize << "\n";
    std::cerr << "Template difficulty: " << difficulty<< "\n";

    for(auto region : regionInfoList){
        region.printRegion();
    }

    for(auto connection : connectionInfoList){
        std::cerr << "  Connection:\n";
        connection.printConnection();
    }

}