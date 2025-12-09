#include "./ConnectionInfo.h"

ConnectionInfo::ConnectionInfo() : zoneFrom(0), zoneDest(0) {}

void ConnectionInfo::deserializeConnection(const json& connection) {
    int zoneFrom = getOrError<int>(connection, "from_zone");
    int zoneDest = getOrError<int>(connection, "dest_zone");

    this->zoneFrom = zoneFrom;
    this->zoneDest = zoneDest;
}

void ConnectionInfo::printConnection() {
    std::cerr << "      Connection From: " << zoneFrom << "\n";
    std::cerr << "      Connection Dest: " << zoneDest << "\n";
}