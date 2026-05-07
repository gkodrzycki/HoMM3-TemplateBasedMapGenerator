#pragma once

#include "../global/Global.hpp"
#include "./ConnectionTemplate.hpp"
#include "./ZoneTemplate.hpp"

class TemplateInfo {
  public:
    TemplateInfo(int debug);

    int getDebug() const;
    // Pack metadata
    string getName() const;
    string getDescription() const;
    int getMaxBattleRounds() const;
    string getTownSelection() const;
    string getHeroes() const;
    bool getMirror() const;
    string getTags() const;
    bool getForbidHiringHeroes() const;

    // Map settings
    string getMapName() const;
    int getMinimumSize() const;
    int getMaximumSize() const;
    string getMapObjects() const;
    bool getSpecialWeeksDisabled() const;
    bool getSpellResearch() const;
    string getArtifacts() const;
    string getComboArts() const;
    string getSpells() const;
    string getSecondarySkills() const;
    string getRockBlocks() const;
    int getZoneSparseness() const;
    bool getAnarchy() const;

    const vector<ZoneTemplate> &getZones() const;
    const vector<ConnectionTemplate> &getConnections() const;

    // Looks up a zone by id; throws if not found.
    const ZoneTemplate &getZoneById(int id) const;

    void deserialize(const json &tmpl);
    void print() const;

  private:
    int debug;
    // Pack
    string name;
    string description;
    string townSelection;
    string heroes;
    bool mirror;
    string tags;
    int maxBattleRounds;
    bool forbidHiringHeroes;

    // Map
    string mapName;
    int minimumSize;
    int maximumSize;
    string artifacts;
    string comboArts;
    string spells;
    string secondarySkills;
    string mapObjects;
    string rockBlocks;
    int zoneSparseness;
    bool specialWeeksDisabled;
    bool spellResearch;
    bool anarchy;

    vector<ZoneTemplate> zones;
    vector<ConnectionTemplate> connections;
};
