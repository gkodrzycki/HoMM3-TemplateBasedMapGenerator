#pragma once

#include "../global/Global.hpp"
#include "./TemplateConnection.hpp"
#include "./TemplateZone.hpp"

class TemplateInfo {
  public:
    TemplateInfo();

    // Pack metadata
    string getName() const;
    string getDescription() const;
    int getMaxBattleRounds() const;

    // Map settings
    string getMapName() const;
    int getMinimumSize() const;
    int getMaximumSize() const;
    string getMapObjects() const;
    bool getSpecialWeeksDisabled() const;
    bool getSpellResearch() const;

    const vector<TemplateZone> &getZones() const;
    const vector<TemplateConnection> &getConnections() const;

    // Looks up a zone by id; throws if not found.
    const TemplateZone &getZoneById(int id) const;

    void deserialize(const json &tmpl);
    void print() const;

  private:
    // Pack
    string name;
    string description;
    int maxBattleRounds;

    // Map
    string mapName;
    int minimumSize;
    int maximumSize;
    string mapObjects;
    bool specialWeeksDisabled;
    bool spellResearch;

    vector<TemplateZone> zones;
    vector<TemplateConnection> connections;
};
