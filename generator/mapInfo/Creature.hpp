#pragma once

#include "../global/int3.hpp"
#include "./Object.hpp"

class Creature : public Object {
  public:
    Creature();
    Creature(const string &name, const int3 &position, const int &quantity,
             const string &disposition, const bool &never_flees, const bool &does_not_grow);

    void setQuantity(int quantity);
    int getQuantity() const;

    void setPosition(int3 position);
    int3 getPosition() const;

    void setName(string name);
    string getName() const;
    void setDisposition(string disposition);
    string getDisposition() const;

    void setNeverFlees(bool never_flees);
    bool getNeverFlees() const;
    void setDoesNotGrow(bool does_not_grow);
    bool getDoesNotGrow() const;

    void printObject() const override;

  private:
    int quantity;

    int3 position;

    string name;
    string disposition;

    bool never_flees;
    bool does_not_grow;
};
