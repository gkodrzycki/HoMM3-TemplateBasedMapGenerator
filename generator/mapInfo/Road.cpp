#include "./Road.hpp"

Road::Road() : Object(int3(1, 1, 1), "Road") { roadTier = 0; }

Road::Road(const int &roadTier, const int3 &position, const string &name)
    : Object(position, name, int3(1, 1, 1)) {
    this->roadTier = roadTier;
}

void Road::setRoadTier(int roadTier) { this->roadTier = roadTier; }

int Road::getRoadTier() const { return roadTier; }

void Road::printObject() const {
    cerr << "Object '" << getName() << "' Road tier '" << roadTier << "' at "
         << getPosition().toString() << "\n";
}
