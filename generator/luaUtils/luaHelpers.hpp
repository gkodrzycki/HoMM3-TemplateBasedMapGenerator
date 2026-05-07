#ifndef LUA_HELPERS_HPP
#define LUA_HELPERS_HPP

#include "../global/Global.hpp"
#include "../global/Random.hpp"
#include "../map/Map.hpp"

#include </usr/include/lua/lua.hpp>

string encodeMapSize(int width, int height);

void AddHeader(ofstream &luaFile);
void AddPlayer(ofstream &luaFile, int playerId);
void AddTown(ofstream &luaFile, shared_ptr<Town> town, bool is_main = true);
void AddTowns(ofstream &luaFile, Map &map);
void AddTerrain(ofstream &luaFile, string terrain = "GRASS");
void AddTerrainTiles(ofstream &luaFile, Map &map);
void AddBorderObstacles(ofstream &luaFile, Map &map);
void AddRoad(ofstream &luaFile, const int &tier, int3 pos);
void AddRoads(ofstream &luaFile, Map &map);
void AddMonolith(ofstream &luaFile, int3 pos, int connectionNumber);
void AddMonoliths(ofstream &luaFile, Map &map);
void AddCreature(ofstream &luaFile, shared_ptr<Creature> creature);
void AddCreatures(ofstream &luaFile, Map &map);
void AddMine(ofstream &luaFile, shared_ptr<Mine> mine);
void AddMines(ofstream &luaFile, Map &map);
void AddResource(ofstream &luaFile, shared_ptr<Resource> resource);
void AddArtifact(ofstream &luaFile, shared_ptr<Artifact> artifact);
void AddTreasures(ofstream &luaFile, Map &map);
// void AddBuildingTreasure(ofstream& luaFile, Treasure treasure);
// void AddSign(ofstream& luaFile, string text, int x, int y, int z);

#endif
