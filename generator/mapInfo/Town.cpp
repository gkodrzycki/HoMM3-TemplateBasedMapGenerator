#include "./Town.hpp"

Town::Town() : Object(int3(1, 1, 1), "Town", int3(5, 3, 1)) {
    faction = Faction::NONE;
    hasFort = false;
}

Town::Town(Faction faction) : Object(int3(1, 1, 1), "Town", int3(5, 3, 1)) {
    this->faction = faction;
    this->hasFort = false;
}

Town::Town(Faction faction, const string &owner, bool hasFort, const int3 &position,
           const string &name)
    : Object(position, name, int3(5, 3, 1)) {
    this->faction = faction;
    this->owner   = owner;
    this->hasFort = hasFort;
}

void Town::setFaction(Faction faction) { this->faction = faction; }

Faction Town::getFaction() { return faction; }

void Town::setOwner(const int &owner) { this->owner = owner; }

int Town::getOwner() { return owner; }

void Town::setHasFort(bool hasFort) { this->hasFort = hasFort; }

bool Town::getHasFort() const { return hasFort; }

void Town::printObject() const {
    cerr << "Town '" << getName() << "' owned by '" << owner << "' ("
         << getEnumName<Faction>(faction) << ") at " << getPosition().toString() << "\n";
}
