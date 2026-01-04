#include "./luaHelpers.hpp"

string encodeMapSize(string size) {
    if (size == "S")
        return "SMALL";
    if (size == "M")
        return "MEDIUM";
    if (size == "L")
        return "LARGE";
    if (size == "XL")
        return "EXTRA_LARGE";
    return "UNKNOWN_SIZE";
}

// @function    AddHeader
// @tparam      ofstream    luaFile     file where we save lua script.
void AddHeader(ofstream &luaFile) {
    luaFile << R"(
package.cpath = package.cpath .. ';dist/?.so;../dist/?.so'
local homm3lua = require('homm3lua'))";
    luaFile << "\n";
}

// @function    AddPlayer
// @tparam      ofstream    luaFile     file where we save lua script.
// @tparam      string      playerId    player ID.
void AddPlayer(ofstream &luaFile, string playerId) {
    luaFile << "instance:player(homm3lua.PLAYER_" << playerId << ")\n";
}

// @function    AddTown
// @tparam      ofstream    luaFile     file where we save lua script.
// @tparam      Town        town        completed town object.
// @tparam      boolean     is_main     tells if is main town.
void AddTown(ofstream &luaFile, shared_ptr<Town> town, bool is_main) {

    string nameOfObject = "TOWN_" + factionToString(town->getFaction());
    string ID           = town->getOwner();
    int X               = town->getPosition().x;
    int Y               = town->getPosition().y;

    luaFile << "instance:town(homm3lua." << nameOfObject << ", {x=" << X << ", y=" << Y
            << ", z=0}, homm3lua.PLAYER_" << ID << ", " << is_main << ")\n";
}

// @function    AddTowns
// @tparam      ofstream    luaFile     file where we save lua script.
// @tparam      Map         map         object of map class with finished setup
void AddTowns(ofstream &luaFile, Map &map) {
    ObjectVector objectVector = map.getObjectVector();
    set<string> addedPlayers;

    for (auto object : objectVector) {
        if (auto town = std::dynamic_pointer_cast<Town>(object)) {
            string playerID = town->getOwner();
            if (addedPlayers.find(playerID) == addedPlayers.end()) {
                addedPlayers.insert(playerID);
                AddPlayer(luaFile, playerID);
            }
            AddTown(luaFile, town, true);
        }
    }
}

// @function    AddTerrain
// @tparam      ofstream    luaFile     file where we save lua script.
// @tparam      string      terrain     type of terrain, GRASS by default, See TERRAIN_*.
void AddTerrain(std::ofstream &luaFile, std::string terrain) {
    luaFile << "instance:terrain(homm3lua.TERRAIN_" << terrain << ")\n";
}

// @function    AddTerrainTiles
// @tparam      ofstream    luaFile     file where we save lua script.
// @tparam      Map         map         object of map class with finished setup.
void AddTerrainTiles(std::ofstream &luaFile, Map &map) {
    luaFile << "instance:terrain(function (x, y, z)\n";

    int width  = map.getWidth();
    int height = map.getHeight();
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            auto tilePtr   = map.getTile(int3(x, y, 0));
            string terrain = tilePtr->getTerrain();
            transform(terrain.begin(), terrain.end(), terrain.begin(), ::toupper);

            luaFile << "if x == " << x << " and y == " << y << " then return homm3lua.TERRAIN_"
                    << terrain << " end\n";
        }
    }

    luaFile << "end)\n";
}

// @function    AddObstacle
// @tparam      ofstream    luaFile          file where we save lua script.
// @tparam      string      obstacle         type of obstacle.
// @tparam      int3        pos              position of obstacle.
void AddObstacle(ofstream &luaFile, const string &obstacle, int3 pos) {
    luaFile << "instance:obstacle('" << obstacle << "', {x=" << pos.x << ", y=" << pos.y
            << ", z=" << pos.z << "})\n";
}

// @function    AddBorderObstacles
// @tparam      ofstream    luaFile     file where we save lua script.
// @tparam      Map         map         object of map class with finished setup.
void AddBorderObstacles(ofstream &luaFile, Map &map) {
    ObjectVector objectVector = map.getObjectVector();

    for (const auto &object : objectVector) {
        if (auto obstacle = std::dynamic_pointer_cast<Obstacle>(object)) {
            AddObstacle(luaFile, obstacle->getObstacleName(), obstacle->getPosition());
        }
    }

    // Example Obstacles
    //  Lava Flow
    //  Mountain4

    // Rock_Rough10?
    //  Pine Trees
    //  Mushrooms
    //  Mountain
}

// @function    AddRoad
// @tparam      ofstream    luaFile          file where we save lua script.
// @tparam      int         tier             tier of road.
// @tparam      int3        pos              position of road.
void AddRoad(ofstream &luaFile, const int &tier, int3 pos) {
    luaFile << "    if x == " << pos.x << " and y == " << pos.y << " then return nil, " << tier
            << " end\n";
}

// @function    AddRoads
// @tparam      ofstream    luaFile         file where we save lua script.
// @tparam      Map         map             object of map class with finished setup.
void AddRoads(ofstream &luaFile, Map &map) {
    luaFile << "-- Dynamic terrain adjustments for linear paths between towns\n";
    luaFile << "instance:terrain(function (x, y, z)\n";

    ObjectVector objectVector = map.getObjectVector();

    for (const auto &object : objectVector) {
        if (auto road = std::dynamic_pointer_cast<Road>(object)) {
            AddRoad(luaFile, road->getRoadTier(), road->getPosition());
        }
    }

    luaFile << "    return nil\n"; // Default terrain
    luaFile << "end)\n";
}

// @function    AddCreature
// @tparam      ofstream    luaFile          file where we save lua script.
// @tparam      Creature    creature         Creature object to place.

// @tparam      string      name             name of creature.
// @tparam      int3        position         position of creature.
// @tparam      integer     quantity         quantity of creature.
// @tparam      string      disposition      disposition of creature.
// @tparam      boolean     neverFlees      if creature never flees.
// @tparam      boolean     doesNotGrow    if creature does not grow.
void AddCreature(ofstream &luaFile, const Creature &creature) {
    string disp = creature.getDisposition();
    transform(disp.begin(), disp.end(), disp.begin(), ::toupper);

    luaFile << "instance:creature('" << creature.getCreatureName()
            << "', {x=" << creature.getPosition().x << ", y=" << creature.getPosition().y
            << ", z=" << creature.getPosition().z << "}, " << creature.getQuantity()
            << ", homm3lua.DISPOSITION_" << disp << ", "
            << (creature.getNeverFlees() ? "true" : "false") << ", "
            << (creature.getDoesNotGrow() ? "true" : "false") << ")\n";
}

// @function    AddCreatures
// @tparam      ofstream    luaFile         file where we save lua script.
// @tparam      Map         map             object of map class with finished setup.
void AddCreatures(ofstream &luaFile, Map &map) {
    CreatureVector creatureVector = map.getCreatureVector();
    for (const auto &creature : creatureVector) {
        if (auto creaturePtr = std::dynamic_pointer_cast<Creature>(creature)) {
            AddCreature(luaFile, *creaturePtr);
        }
    }
}

// // @function    AddMine
// // @tparam      ofstream    luaFile     file where we save lua script.
// // @tparam      Mine        mine        completed mine object.
// // @tparam      Map         map        object of map class with finished
// setup. void AddMine(ofstream& luaFile, Mine mine, Map &map){
//     i32 x = mine.getPosition().x;
//     i32 y = mine.getPosition().y;
//     i32 z = mine.getPosition().z;

//     i32 owner_id = mine.getOwner();

//     string mineType = mineTypeToString(mine.getMineType());

//     string owner = owner_id <= 0 ? "OWNER_NEUTRAL" : "PLAYER_" +
//     to_string(owner_id); luaFile << "instance:mine(homm3lua." << mineType <<
//     ", {x=" << x << ", y=" << y << ", z=" << z << "}, homm3lua." << owner <<
//     ")\n";

// }

// // @function    AddResource
// // @tparam      ofstream    luaFile          file where we save lua script.
// // @tparam      Treasure    treasure         Treasure object to place.
// void AddResource(ofstream& luaFile, Treasure treasure){
//     string treasureType = treasureTypeToString(treasure.getTreasureType());

//     int x = treasure.getPosition().x;
//     int y = treasure.getPosition().y;
//     int z = treasure.getPosition().z;

//     int quantity = treasure.getQuantity();

//     luaFile << "instance:resource(homm3lua." << treasureType << ", {x=" << x
//     << ", y=" << y << ", z=" << z << "}, " << quantity << ")\n";
// }

// // @function    AddArtifact
// // @tparam      ofstream    luaFile          file where we save lua script.
// // @tparam      Treasure    treasure         Treasure object to place.
// void AddArtifact(ofstream& luaFile, Treasure treasure){
//     string treasureType = treasureTypeToString(treasure.getTreasureType());

//     int x = treasure.getPosition().x;
//     int y = treasure.getPosition().y;
//     int z = treasure.getPosition().z;

//     luaFile << "instance:artifact(homm3lua." << treasureType << ", {x=" << x
//     << ", y=" << y << ", z=" << z << "})\n";
// }

// // @function    AddBuildingTreasure
// // @tparam      ofstream    luaFile          file where we save lua script.
// // @tparam      Treasure    treasure         Treasure object to place.
// void AddBuildingTreasure(ofstream& luaFile, Treasure treasure) {
//     string treasureType = treasureTypeToString(treasure.getTreasureType());

//     int x = treasure.getPosition().x;
//     int y = treasure.getPosition().y;
//     int z = treasure.getPosition().z;

//     luaFile << "instance:obstacle('" << treasureType << "', {x=" << x << ",
//     y=" << y << ", z=" << z << "})\n";

// }

// // @function    AddSign
// // @tparam      ofstream    luaFile          file where we save lua script.
// // @tparam      string      text             text on sign.
// // @tparam      integer     x                position on x axis of sign.
// // @tparam      integer     y                position on y axis of sign.
// // @tparam      integer     z                position on z axis of sign.
// void AddSign(ofstream& luaFile, string text, int x, int y, int z){
//     luaFile << "instance:sign('" << text << "', {x=" << x << ", y=" << y <<
//     ", z=" << z << "})\n";
// }

// // @function    AddMapObjects
// // @tparam      ofstream        luaFile         file where we save lua script.
// // @tparam      Map             map             object of map class with
// finished setup. void AddMapObjects(ofstream &luaFile, Map& map){

//     luaFile << "-- Place 1 way monoliths\n";

//     MapObjects mapObjects = map.getMapObjects();

//     for (auto object : mapObjects)
//     {
//         string obstacle = object.getName();
//         auto pos = object.getPosition();

//         luaFile << "instance:obstacle('" << obstacle << "', {x=" << pos.x <<
//         ", y=" << pos.y << ", z=" << pos.z << "})\n";
//     }
// }
