#include "./Creature.hpp"

Creature::Creature() : Object(int3(1, 1, 1), "Creature") {
    quantity     = 0;
    creatureName = "";
    disposition  = "";
    neverFlees   = false;
    doesNotGrow  = false;
};

Creature::Creature(const string &creatureName, const int3 &position, int quantity,
                   const string &disposition, bool neverFlees, bool doesNotGrow, const string &name)
    : Object(position, name) {
    this->quantity     = quantity;
    this->creatureName = creatureName;
    this->disposition  = disposition;
    this->neverFlees   = neverFlees;
    this->doesNotGrow  = doesNotGrow;
}

void Creature::setQuantity(int quantity) { this->quantity = quantity; }
int Creature::getQuantity() const { return quantity; }

void Creature::setCreatureName(const string &creatureName) { this->creatureName = creatureName; }
string Creature::getCreatureName() const { return creatureName; }
void Creature::setDisposition(const string &disposition) { this->disposition = disposition; }
string Creature::getDisposition() const { return disposition; }

void Creature::setNeverFlees(bool neverFlees) { this->neverFlees = neverFlees; }
bool Creature::getNeverFlees() const { return neverFlees; }
void Creature::setDoesNotGrow(bool doesNotGrow) { this->doesNotGrow = doesNotGrow; }
bool Creature::getDoesNotGrow() const { return doesNotGrow; }

void Creature::printObject() const {
    cerr << "Creature '" << creatureName << "' x" << quantity << " disposition '" << disposition
         << "' never flees: " << (neverFlees ? "true" : "false")
         << " does not grow: " << (doesNotGrow ? "true" : "false") << " at "
         << getPosition().toString() << "\n";
}
