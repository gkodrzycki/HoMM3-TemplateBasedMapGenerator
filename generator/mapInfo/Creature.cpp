#include "./Creature.hpp"

Creature::Creature() : Object(int3(1, 1, 1), "Creature") {
    quantity     = 0;
    creatureType = CreatureType::CREATURE_UNKNOWN;
    disposition  = "";
    neverFlees   = false;
    doesNotGrow  = false;
};

Creature::Creature(CreatureType creatureType, const int3 &position, int quantity,
                   const string &disposition, bool neverFlees, bool doesNotGrow, const string &name)
    : Object(position, name) {
    this->quantity     = quantity;
    this->creatureType = creatureType;
    this->disposition  = disposition;
    this->neverFlees   = neverFlees;
    this->doesNotGrow  = doesNotGrow;
}

void Creature::setQuantity(int quantity) { this->quantity = quantity; }
int Creature::getQuantity() const { return quantity; }

void Creature::setCreatureType(CreatureType creatureType) { this->creatureType = creatureType; }
CreatureType Creature::getCreatureType() const { return creatureType; }
void Creature::setDisposition(const string &disposition) { this->disposition = disposition; }
string Creature::getDisposition() const { return disposition; }

void Creature::setNeverFlees(bool neverFlees) { this->neverFlees = neverFlees; }
bool Creature::getNeverFlees() const { return neverFlees; }
void Creature::setDoesNotGrow(bool doesNotGrow) { this->doesNotGrow = doesNotGrow; }
bool Creature::getDoesNotGrow() const { return doesNotGrow; }

void Creature::printObject() const {
    cerr << "Creature '" << getEnumName<CreatureType>(creatureType) << "' x" << quantity
         << " disposition '" << disposition << "' never flees: " << (neverFlees ? "true" : "false")
         << " does not grow: " << (doesNotGrow ? "true" : "false") << " at "
         << getPosition().toString() << "\n";
}
