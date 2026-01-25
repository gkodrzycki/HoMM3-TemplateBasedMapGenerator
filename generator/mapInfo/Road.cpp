#include "./Road.hpp"

Road::Road() : Object(int3(1, 1, 1), "Road") { roadTier = 0; }

Road::Road(const int &roadTier, const vector<int3> &path, const string &name)
    : Object(path.front(), name, int3(1, 1, 1)) {
    this->roadTier = roadTier;
    this->path     = path;
}

void Road::setRoadTier(int roadTier) { this->roadTier = roadTier; }
void Road::setPath(const vector<int3> &path) { this->path = path; }

int Road::getRoadTier() const { return roadTier; }
vector<int3> Road::getPath() const { return path; }

void Road::printObject() const {
    cerr << "Object '" << getName() << "' Road tier '" << roadTier << "' starting at "
         << getPosition().toString() << "\n";
}
