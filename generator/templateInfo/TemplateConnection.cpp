#include "./TemplateConnection.hpp"

TemplateConnection::TemplateConnection()
    : zone1(0), zone2(0), value(0), wide(false), borderGuard(false), fictive(false),
      portalRepulsion(0) {}

int TemplateConnection::getZone1() const { return zone1; }
int TemplateConnection::getZone2() const { return zone2; }
int TemplateConnection::getValue() const { return value; }
bool TemplateConnection::isWide() const { return wide; }
bool TemplateConnection::hasBorderGuard() const { return borderGuard; }
bool TemplateConnection::isFictive() const { return fictive; }
int TemplateConnection::getPortalRepulsion() const { return portalRepulsion; }
const TemplateConnectionRestrictions &TemplateConnection::getRestrictions() const {
    return restrictions;
}

void TemplateConnection::deserialize(const json &connection) {
    const auto &zones = getOrError<json>(connection, "Zones");
    zone1             = getOrError<int>(zones, "Zone 1");
    zone2             = getOrError<int>(zones, "Zone 2");

    if (connection.contains("Options")) {
        const auto &opts = connection.at("Options");
        value            = getOrDefault<int>(opts, "Value", 0);
        wide             = getOrDefault<bool>(opts, "Wide", false);
        borderGuard      = getOrDefault<bool>(opts, "Border Guard", false);
        fictive          = getOrDefault<bool>(opts, "Fictive", false);
        portalRepulsion  = getOrDefault<int>(opts, "Portal repulsion", 0);
    }

    if (connection.contains("Restrictions")) {
        const auto &r                  = connection.at("Restrictions");
        restrictions.minHumanPositions = getOrDefault<int>(r, "Minimum human positions", 0);
        restrictions.maxHumanPositions = getOrDefault<int>(r, "Maximum human positions", 0);
        restrictions.minTotalPositions = getOrDefault<int>(r, "Minimum total positions", 0);
        restrictions.maxTotalPositions = getOrDefault<int>(r, "Maximum total positions", 0);
    }
}

void TemplateConnection::print() const {
    cerr << "      Zone 1: " << zone1 << "\n";
    cerr << "      Zone 2: " << zone2 << "\n";
    cerr << "      Value: " << value << "\n";
    if (wide)
        cerr << "      Wide: true\n";
    if (borderGuard)
        cerr << "      Border Guard: true\n";
    if (fictive)
        cerr << "      Fictive: true\n";
    if (portalRepulsion)
        cerr << "      Portal repulsion: " << portalRepulsion << "\n";
    cerr << "      Restrictions: human " << restrictions.minHumanPositions << "-"
         << restrictions.maxHumanPositions << "  total " << restrictions.minTotalPositions << "-"
         << restrictions.maxTotalPositions << "\n";
}
