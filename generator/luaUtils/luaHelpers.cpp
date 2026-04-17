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
// @tparam      ofstream                luaFile     file where we save lua script.
// @tparam      shared_ptr<Town>        town        pointer to town.
// @tparam      boolean                 is_main     tells if is main town.
void AddTown(ofstream &luaFile, shared_ptr<Town> town, bool is_main) {
    string nameOfObject = "TOWN_" + getEnumName<Faction>(town->getFaction());
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

    RoadVector roadVector = map.getRoadVector();

    for (const auto &road : roadVector) {
        int tier          = road->getRoadTier();
        vector<int3> path = road->getPath();
        for (auto pos : path) {
            AddRoad(luaFile, tier, pos);
        }
    }

    luaFile << "    return nil\n"; // Default terrain
    luaFile << "end)\n";
}

// @function    AddMonolith
// @tparam      ofstream    luaFile          file where we save lua script.
// @tparam      int3        pos              position of monolith.
// @param       connectionNumber             connection number.
void AddMonolith(ofstream &luaFile, int3 pos, int connectionNumber) {
    luaFile << "instance:obstacle('" << "Monolith Two Way" << connectionNumber << "', {x=" << pos.x
            << ", y=" << pos.y << ", z=" << pos.z << "})\n";
}

// @function    AddMonoliths
// @tparam      ofstream    luaFile         file where we save lua script.
// @tparam      Map         map             object of map class with finished setup.
void AddMonoliths(ofstream &luaFile, Map &map) {
    MonolithVector monolithVector = map.getMonolithVector();

    int counter = 0;
    for (const auto &monoliths : monolithVector) {
        if (counter >= 5) {
            throw out_of_range("Too many monoliths, only 5 connections are supported. Found " +
                               to_string(counter + 1) + " on seed " +
                               to_string(map.getRNG().getOriginalSeed()) + "\n");
        }
        AddMonolith(luaFile, monoliths.first->getPosition(), counter);
        AddMonolith(luaFile, monoliths.second->getPosition(), counter);
        counter++;
    }
}

// @function    AddCreature
// @tparam      ofstream                luaFile          file where we save lua script.
// @tparam      shared_ptr<Creature>    creature         pointer to creature.
void AddCreature(ofstream &luaFile, shared_ptr<Creature> creature) {
    string disp = creature->getDisposition();
    transform(disp.begin(), disp.end(), disp.begin(), ::toupper);

    string creatureType = getEnumName<CreatureType>(creature->getCreatureType());

    transform(creatureType.begin(), creatureType.end(), creatureType.begin(), ::tolower);

    replace(creatureType.begin(), creatureType.end(), '_', ' ');
    creatureType[0] = toupper(creatureType[0]);
    for (size_t i = 1; i < creatureType.length(); i++) {
        if (creatureType[i - 1] == ' ') {
            creatureType[i] = toupper(creatureType[i]);
        }
    }

    luaFile << "instance:creature('" << creatureType << "', {x=" << creature->getPosition().x
            << ", y=" << creature->getPosition().y << ", z=" << creature->getPosition().z << "}, "
            << creature->getQuantity() << ", homm3lua.DISPOSITION_" << disp << ", "
            << (creature->getNeverFlees() ? "true" : "false") << ", "
            << (creature->getDoesNotGrow() ? "true" : "false") << ")\n";
}

// @function    AddCreatures
// @tparam      ofstream    luaFile         file where we save lua script.
// @tparam      Map         map             object of map class with finished setup.
void AddCreatures(ofstream &luaFile, Map &map) {
    CreatureVector creatureVector = map.getCreatureVector();
    for (const auto &creature : creatureVector) {
        AddCreature(luaFile, creature);
    }
}

// @function    AddMine
// @tparam      ofstream                luaFile     file where we save lua script.
// @tparam      shared_ptr<Mine>        mine        completed mine object.
void AddMine(ofstream &luaFile, shared_ptr<Mine> mine) {
    int owner_id = mine->getOwner();
    string owner = owner_id <= 0 ? "OWNER_NEUTRAL" : "PLAYER_" + to_string(owner_id);
    luaFile << "instance:mine(homm3lua." << getEnumName<MineType>(mine->getMineType())
            << ", {x=" << mine->getPosition().x << ", y=" << mine->getPosition().y
            << ", z=" << mine->getPosition().z << "}, homm3lua." << owner << ")\n";
}

// @function    AddMines
// @tparam      ofstream    luaFile     file where we save lua script.
// @tparam      Map         map         object of map class with finished setup
void AddMines(ofstream &luaFile, Map &map) {
    ObjectVector objectVector = map.getObjectVector();
    for (auto object : objectVector) {
        if (auto mine = std::dynamic_pointer_cast<Mine>(object)) {
            AddMine(luaFile, mine);
        }
    }
}

// @function    AddResource
// @tparam      ofstream                luaFile         file where we save lua script.
// @tparam      shared_ptr<Resource>    resource        completed resource object.
void AddResource(ofstream &luaFile, shared_ptr<Resource> resource) {
    luaFile << "instance:resource(homm3lua."
            << getEnumName<ResourceType>(resource->getResourceType())
            << ", {x=" << resource->getPosition().x << ", y=" << resource->getPosition().y
            << ", z=" << resource->getPosition().z << "}, " << resource->getQuantity() << ")\n";
}

// @function    AddArtifact
// @tparam      ofstream                luaFile          file where we save lua script.
// @tparam      shared_ptr<Artifact>    artifact         completed artifact object.
void AddArtifact(ofstream &luaFile, shared_ptr<Artifact> artifact) {
    luaFile << "instance:artifact(homm3lua."
            << getEnumName<ArtifactType>(artifact->getArtifactType())
            << ", {x=" << artifact->getPosition().x << ", y=" << artifact->getPosition().y
            << ", z=" << artifact->getPosition().z << "})\n";
}

// @function    AddTreasures
// @tparam      ofstream    luaFile     file where we save lua script.
// @tparam      Map         map         object of map class with finished setup.
void AddTreasures(ofstream &luaFile, Map &map) {
    TreasureVector treasureVector = map.getTreasureVector();
    for (auto treasure : treasureVector) {
        if (auto artifact = dynamic_pointer_cast<Artifact>(treasure)) {
            AddArtifact(luaFile, artifact);
        } else if (auto resource = dynamic_pointer_cast<Resource>(treasure)) {
            AddResource(luaFile, resource);
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
