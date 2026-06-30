#include "./Obstacle.hpp"

Obstacle::Obstacle() : Object(int3(1, 1, 1), "Obstacle") { obstacleName = "Pine Trees"; }

Obstacle::Obstacle(const string &obstacleName, const int3 &position, const string &name)
    : Object(position, name) {
    this->obstacleName = obstacleName;
}

Obstacle::Obstacle(const string &obstacleName, const int3 &position, const string &name,
                   const int3 &size, const int3 &entryPoint, const vector<string> &realSize)
    : Object(position, name, size, entryPoint, realSize) {
    this->obstacleName = obstacleName;
}

void Obstacle::setObstacleName(const string &obstacleName) { this->obstacleName = obstacleName; }
const string Obstacle::getObstacleName() const { return obstacleName; }

void Obstacle::printObject() const {
    cerr << "Object '" << getName() << "' obstacle name '" << obstacleName << "' at "
         << getPosition().toString() << "\n";
}
