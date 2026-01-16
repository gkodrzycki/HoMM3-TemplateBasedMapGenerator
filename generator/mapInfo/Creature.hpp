#pragma once

#include "../global/int3.hpp"
#include "./CreatureType.hpp"
#include "./Object.hpp"

class Creature : public Object {
  public:
    Creature();
    Creature(CreatureType creatureType, const int3 &position, int quantity,
             const string &disposition, bool neverFlees, bool doesNotGrow, const string &name);

    void setQuantity(int quantity);
    int getQuantity() const;

    void setCreatureType(CreatureType creatureType);
    CreatureType getCreatureType() const;
    void setDisposition(const string &disposition);
    string getDisposition() const;

    void setNeverFlees(bool neverFlees);
    bool getNeverFlees() const;
    void setDoesNotGrow(bool doesNotGrow);
    bool getDoesNotGrow() const;

    void printObject() const override;

  private:
    int quantity;

    CreatureType creatureType;
    string disposition;

    bool neverFlees;
    bool doesNotGrow;
};
