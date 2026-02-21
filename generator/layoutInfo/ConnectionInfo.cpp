#include "./ConnectionInfo.hpp"

ConnectionInfo::ConnectionInfo() : zoneFrom(0), zoneDest(0) {}

int ConnectionInfo::getZoneFrom() { return zoneFrom; }

int ConnectionInfo::getZoneDest() { return zoneDest; }

void ConnectionInfo::deserializeConnection(const json &connection) {
    int zoneFrom = getOrError<int>(connection, "fromZone");
    int zoneDest = getOrError<int>(connection, "destZone");

    this->zoneFrom = zoneFrom;
    this->zoneDest = zoneDest;
}

void ConnectionInfo::printConnection() {
    cerr << "      Connection From: " << zoneFrom << "\n";
    cerr << "      Connection Dest: " << zoneDest << "\n";
}
