#include "./Creature.hpp"

Creature::Creature() : Object(int3(1, 1, 1), "Creature") {
    quantity      = 0;
    position      = int3(0, 0, 0);
    name          = "";
    disposition   = "";
    never_flees   = false;
    does_not_grow = false;
};

Creature::Creature(const string &name, const int3 &position, const int &quantity,
                   const string &disposition, const bool &never_flees, const bool &does_not_grow)
    : Object(position, name) {
    this->quantity      = quantity;
    this->disposition   = disposition;
    this->never_flees   = never_flees;
    this->does_not_grow = does_not_grow;
};

void Creature::setQuantity(int quantity) { this->quantity = quantity; }
int Creature::getQuantity() const { return quantity; }

void Creature::setPosition(int3 position) { this->position = position; }
int3 Creature::getPosition() const { return position; }

void Creature::setName(string name) { this->name = name; }
string Creature::getName() const { return name; }
void Creature::setDisposition(string disposition) { this->disposition = disposition; }
string Creature::getDisposition() const { return disposition; }

void Creature::setNeverFlees(bool never_flees) { this->never_flees = never_flees; }
bool Creature::getNeverFlees() const { return never_flees; }
void Creature::setDoesNotGrow(bool does_not_grow) { this->does_not_grow = does_not_grow; }
bool Creature::getDoesNotGrow() const { return does_not_grow; }

void Creature::printObject() const {
    cerr << "Creature '" << name << "' x" << quantity << " disposition '" << disposition
         << "' never flees: " << (never_flees ? "true" : "false")
         << " does not grow: " << (does_not_grow ? "true" : "false") << " at "
         << position.toString() << "\n";
}
