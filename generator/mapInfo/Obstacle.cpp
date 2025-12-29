#include "./Obstacle.hpp"

Obstacle::Obstacle() : Object(int3(1, 1, 1), "Obstacle") { obstacleName = "Pine Trees"; }

Obstacle::Obstacle(const string &obstacleName, int3 position, const string &name)
    : Object(position, name) {
    this->obstacleName = obstacleName;
}

void Obstacle::setObstacleName(const string &obstacleName) { this->obstacleName = obstacleName; }
string Obstacle::getObstacleName() { return obstacleName; }

void Obstacle::printObject() const {
    cerr << "Object '" << getName() << "' obstacle name '" << obstacleName << "' at "
         << getPosition().toString() << "\n";
}
