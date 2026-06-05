#include "./Object.hpp"

Object::Object() {
    position   = int3(0, 0, 0);
    name       = "";
    size       = int3(1, 1, 1);
    realSize   = {"1"};
    entryPoint = int3(0, 0, 0);
}

Object::Object(int3 position, string name) {
    this->position   = position;
    this->name       = name;
    this->size       = int3(1, 1, 1);
    this->realSize   = {"1"};
    this->entryPoint = int3(0, 0, 0);
}

Object::Object(int3 position, string name, int3 size) {
    this->name       = name;
    this->size       = size;
    this->position   = position;
    this->entryPoint = int3(0, 0, 0);
    for (int i = 0; i < size.y; i++) {
        string row(size.x, '1');
        realSize.push_back(row);
    }
}
Object::Object(int3 position, string name, int3 size, int3 entryPoint, vector<string> realSize) {
    this->name       = name;
    this->size       = size;
    this->position   = position;
    this->entryPoint = entryPoint;
    this->realSize   = realSize;
}

int3 Object::getEntryPoint() const { return entryPoint; }

int3 Object::getSize() const { return size; }

vector<string> &Object::getRealSize() { return realSize; }

const vector<string> &Object::getRealSize() const { return realSize; }

int Object::getTileCount() const {
    int count = 0;
    for (const auto &row : realSize) {
        for (char c : row) {
            if (c == '1')
                count++;
        }
    }
    return count;
}

int3 Object::getPosition() const { return position; }

string Object::getName() const { return name; }

void Object::setEntryPoint(int3 entryPoint) { this->entryPoint = entryPoint; }

void Object::setRealSize(vector<string> realSize) { this->realSize = realSize; }

void Object::setSize(int3 size) { this->size = size; }

void Object::setPosition(int3 position) { this->position = position; }

void Object::setName(string name) { this->name = name; }

void Object::printObject() const {
    cerr << "Object '" << name << "' at " << position.toString() << " size " << size.toString()
         << "\n";
}
