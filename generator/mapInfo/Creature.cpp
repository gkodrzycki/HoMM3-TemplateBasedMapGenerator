#include "./Creature.hpp"

Creature::Creature() : Object(int3(1, 1, 1), "Creature") {
    quantity      = 0;
    creatureName  = "";
    disposition   = "";
    never_flees   = false;
    does_not_grow = false;
};

Creature::Creature(const string &creatureName, const int3 &position, int quantity,
                   const string &disposition, bool never_flees, bool does_not_grow,
                   const string &name)
    : Object(position, name) {
    this->quantity      = quantity;
    this->creatureName  = creatureName;
    this->disposition   = disposition;
    this->never_flees   = never_flees;
    this->does_not_grow = does_not_grow;
};

void Creature::setQuantity(int quantity) { this->quantity = quantity; }
int Creature::getQuantity() const { return quantity; }

void Creature::setCreatureName(string creatureName) { this->creatureName = creatureName; }
string Creature::getCreatureName() const { return creatureName; }
void Creature::setDisposition(string disposition) { this->disposition = disposition; }
string Creature::getDisposition() const { return disposition; }

void Creature::setNeverFlees(bool never_flees) { this->never_flees = never_flees; }
bool Creature::getNeverFlees() const { return never_flees; }
void Creature::setDoesNotGrow(bool does_not_grow) { this->does_not_grow = does_not_grow; }
bool Creature::getDoesNotGrow() const { return does_not_grow; }

void Creature::printObject() const {
    cerr << "Creature '" << creatureName << "' x" << quantity << " disposition '" << disposition
         << "' never flees: " << (never_flees ? "true" : "false")
         << " does not grow: " << (does_not_grow ? "true" : "false") << " at "
         << getPosition().toString() << "\n";
}
