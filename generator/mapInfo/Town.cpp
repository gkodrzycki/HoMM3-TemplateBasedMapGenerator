#include "./Town.hpp"

Town::Town() : Object(int3(5, 3, 1), "Town") { faction = Faction::NONE; }

Town::Town(Faction faction) : Object(int3(5, 3, 1), "Town") { this->faction = faction; }

Town::Town(Faction faction, const string &owner, int3 position, const string &name)
    : Object(position, name) {
    this->faction = faction;
    this->owner   = owner;
}

void Town::setFaction(Faction faction) { this->faction = faction; }

Faction Town::getFaction() { return faction; }

void Town::setOwner(const string &owner) { this->owner = owner; }

string Town::getOwner() { return owner; }

void Town::printObject() const {
    cerr << "Town '" << getName() << "' owned by '" << owner << "' (" << factionToString(faction)
         << ") at " << getPosition().toString() << "\n";
}
