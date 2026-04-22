#pragma once

#include "../global/Global.hpp"

struct TemplateConnectionRestrictions {
    int minHumanPositions = 0;
    int maxHumanPositions = 0;
    int minTotalPositions = 0;
    int maxTotalPositions = 0;
};

class TemplateConnection {
  public:
    TemplateConnection();

    int getZone1() const;
    int getZone2() const;
    int getValue() const;
    bool isWide() const;
    bool hasBorderGuard() const;
    bool isFictive() const;
    int getPortalRepulsion() const;
    const TemplateConnectionRestrictions &getRestrictions() const;

    void deserialize(const json &connection);
    void print() const;

  private:
    int zone1;
    int zone2;
    int value;
    bool wide;
    bool borderGuard;
    bool fictive;
    int portalRepulsion;
    TemplateConnectionRestrictions restrictions;
};
