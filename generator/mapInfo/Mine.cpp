#include "./Mine.hpp"

Mine::Mine() : Object(int3(1, 1, 1), "Mine") {
    ownerID  = -1;
    mineType = MineType::MINE_SAWMILL;
}

Mine::Mine(const MineType &mineType, int ownerID, const int3 &position, const string &name,
           pair<int, int> mineResourcesCount, shared_ptr<Creature> guardPtr)
    : Object(position, name) {
    this->mineType           = mineType;
    this->ownerID            = ownerID;
    this->mineResourcesCount = mineResourcesCount;
    this->guardPtr           = guardPtr;
}

void Mine::setMineType(const MineType &mineType) { this->mineType = mineType; }
const MineType Mine::getMineType() const { return mineType; }

void Mine::setOwner(const int &ownerID) { this->ownerID = ownerID; }
int Mine::getOwner() const { return ownerID; }

pair<int, int> Mine::getMineResourcesCount() const { return mineResourcesCount; }

shared_ptr<Creature> Mine::getGuardPtr() const { return guardPtr; }

void Mine::printObject() const {
    cerr << "Object '" << getName() << "' Mine type '" << getEnumName<MineType>(mineType)
         << "' owned by player " << ownerID << " at " << getPosition().toString() << "\n";
}
