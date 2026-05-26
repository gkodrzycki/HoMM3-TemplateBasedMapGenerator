#include "./PandoraBox.hpp"

#include <iostream>

PandoraBox::PandoraBox() : Object(int3(0, 0, 0), "PandoraBox") {}

PandoraBox::PandoraBox(const int3 &position, const string &name, const PandoraBoxContent &content)
    : Object(position, name), content(content) {}

void PandoraBox::setContent(const PandoraBoxContent &content) { this->content = content; }

const PandoraBoxContent &PandoraBox::getContent() const { return content; }

void PandoraBox::printObject() const {
    cerr << "PandoraBox at " << getPosition().toString() << " (gold=" << content.gold
         << ", exp=" << content.experience << ")\n";
}
