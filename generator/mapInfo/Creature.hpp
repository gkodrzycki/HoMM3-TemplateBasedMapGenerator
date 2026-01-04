#pragma once

#include "../global/int3.hpp"
#include "./Object.hpp"

class Creature : public Object {
  public:
    Creature();
    Creature(const string &creatureName, const int3 &position, int quantity,
             const string &disposition, bool never_flees, bool does_not_grow, const string &name);

    void setQuantity(int quantity);
    int getQuantity() const;

    void setCreatureName(const string &creatureName);
    string getCreatureName() const;
    void setDisposition(const string &disposition);
    string getDisposition() const;

    void setNeverFlees(bool never_flees);
    bool getNeverFlees() const;
    void setDoesNotGrow(bool does_not_grow);
    bool getDoesNotGrow() const;

    void printObject() const override;

  private:
    int quantity;

    string creatureName;
    string disposition;

    bool never_flees;
    bool does_not_grow;
};
