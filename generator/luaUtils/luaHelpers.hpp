#ifndef LUA_HELPERS_HPP
#define LUA_HELPERS_HPP

#include "../global/Global.hpp"
#include "../global/Random.hpp"
#include "../map/Map.hpp"

#include </usr/include/lua/lua.hpp>

string encodeMapSize(string size);

void AddHeader(ofstream &luaFile);
void AddPlayer(ofstream &luaFile, string playerId);
void AddTown(ofstream &luaFile, shared_ptr<Town> town, bool is_main = true);
void AddTowns(ofstream &luaFile, Map &map);
void AddTerrain(ofstream &luaFile, string terrain = "GRASS");
void AddTerrainTiles(ofstream &luaFile, Map &map);
void AddBorderObstacles(ofstream &luaFile, Map &map);
void AddRoad(ofstream &luaFile, const int &tier, int3 pos);
void AddRoads(ofstream &luaFile, Map &map);
void AddCreature(ofstream &luaFile, shared_ptr<Creature> creature);
void AddCreatures(ofstream &luaFile, Map &map);
void AddMine(ofstream &luaFile, shared_ptr<Mine> mine);
void AddMines(ofstream &luaFile, Map &map);
// void AddCreature(ofstream &luaFile, const string &name, int3 position, int quantity = 1,
//                  const string &disposition = "COMPLIANT", bool neverFlees = true,
//                  bool doesNotGrow = true);
// void AddMine(ofstream& luaFile, Mine mine, Map &map);
// void AddResource(ofstream& luaFile, Treasure treasure);
// void AddArtifact(ofstream& luaFile, Treasure treasure);
// void AddBuildingTreasure(ofstream& luaFile, Treasure treasure);
// void AddObstacle(ofstream& luaFile, string obstacle, int3 pos);
// void AddSign(ofstream& luaFile, string text, int x, int y, int z);
// void AddMapObjects(ofstream &luaFile, Map& map);

#endif
