#pragma once

#include "../global/Global.hpp"

struct ConnectionTemplateRestrictions {
    int minHumanPositions = 0;
    int maxHumanPositions = 0;
    int minTotalPositions = 0;
    int maxTotalPositions = 0;
};

class ConnectionTemplate {
  public:
    ConnectionTemplate();

    int getZone1() const;
    int getZone2() const;
    int getValue() const;
    bool isWide() const;
    bool isBorder() const;
    bool isGuard() const;
    bool isFictive() const;
    int getPortalRepulsion() const;
    string getType() const;
    const ConnectionTemplateRestrictions &getRestrictions() const;

    void deserialize(const json &connection);
    void print() const;

  private:
    int zone1;
    int zone2;
    int value;
    bool wide;
    bool border;
    bool guard;
    bool fictive;
    string type; // possible "ground", "teleport", "random", "underground"
    int portalRepulsion;
    ConnectionTemplateRestrictions restrictions;
};
