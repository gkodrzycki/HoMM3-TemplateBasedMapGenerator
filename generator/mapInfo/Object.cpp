#include "./Object.hpp"

Object::Object() {
    position = int3(0, 0, 0);
    name     = "";
}

Object::Object(int3 position, string name) {
    this->position = position;
    this->name     = name;
    this->size     = int3(1, 1, 1);
}

Object::Object(int3 position, string name, int3 size) {
    this->name = name;
    this->size = size;
    setPosition(position);
}

int3 Object::getSize() const { return size; }

int3 Object::getPosition() const { return position; }

string Object::getName() const { return name; }

void Object::setSize(int3 size) { this->size = size; }

void Object::setPosition(int3 position) {

    this->position = position;
    this->position.x += size.x / 2;
    this->position.y += (size.y - 1) / 2;
}

void Object::setName(string name) { this->name = name; }

void Object::printObject() const {
    cerr << "Object '" << name << "' at " << position.toString() << " size " << size.toString()
         << "\n";
}
