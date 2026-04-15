#include "./ConnectionInfo.hpp"

ConnectionInfo::ConnectionInfo() : zoneFrom(0), zoneDest(0), type("road") {}

int ConnectionInfo::getZoneFrom() { return zoneFrom; }

int ConnectionInfo::getZoneDest() { return zoneDest; }

string ConnectionInfo::getType() { return type; }

void ConnectionInfo::deserializeConnection(const json &connection) {
    int zoneFrom = getOrError<int>(connection, "fromZone");
    int zoneDest = getOrError<int>(connection, "destZone");
    string type  = getOrDefault<string>(connection, "type", "road");

    this->zoneFrom = zoneFrom;
    this->zoneDest = zoneDest;
    this->type     = type;
}

void ConnectionInfo::printConnection() {
    cerr << "      Connection From: " << zoneFrom << "\n";
    cerr << "      Connection Dest: " << zoneDest << "\n";
    cerr << "      Connection Type: " << type << "\n";
}
