#include "./Mine.hpp"

Mine::Mine() : Object(int3(1, 1, 1), "Mine") { mineType = MineType::MINE_SAWMILL; }

Mine::Mine(const MineType &mineType, const int3 &position, const string &name)
    : Object(position, name) {
    this->mineType = mineType;
}

void Mine::setMineType(const MineType &mineType) { this->mineType = mineType; }
const MineType Mine::getMineType() const { return mineType; }

void Mine::printObject() const {
    cerr << "Object '" << getName() << "' Mine type '" << mineTypeToString(mineType) << "' at "
         << getPosition().toString() << "\n";
}
