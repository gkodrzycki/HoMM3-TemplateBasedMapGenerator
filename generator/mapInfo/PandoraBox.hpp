#pragma once

#include "../global/int3.hpp"
#include "./Object.hpp"

struct PandoraBoxContent {
    int gold        = 0;
    int experience  = 0;
    int spellPoints = 0;
    int morale      = 0;
    int luck        = 0;
    int attack      = 0;
    int defense     = 0;
    int spellPower  = 0;
    int knowledge   = 0;
};

class PandoraBox : public Object {
  public:
    PandoraBox();
    PandoraBox(const int3 &position, const string &name, const PandoraBoxContent &content);

    void setContent(const PandoraBoxContent &content);
    const PandoraBoxContent &getContent() const;

    void printObject() const override;

  private:
    PandoraBoxContent content;
};
