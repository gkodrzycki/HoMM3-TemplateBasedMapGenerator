#include "./Treasure.hpp"

Treasure::Treasure() : Object(int3(1, 1, 1), "Treasure") { quantity = 0; };

Treasure::Treasure(int quantity, const int3 &position, const string &name)
    : Object(position, name) {
    this->quantity = quantity;
}

void Treasure::setQuantity(int quantity) { this->quantity = quantity; }
int Treasure::getQuantity() const { return quantity; }

void Treasure::printObject() const {
    cerr << "Treasure '" << "' x" << quantity << " at " << getPosition().toString() << "\n";
}
