#include "../generator/global/Global.hpp"
#include "../generator/luaUtils/luaHelpers.hpp"
#include "../generator/mapInfo/Town.hpp"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

int main() {
    ifstream file("stats/obstacleInfo.json");
    if (!file.is_open()) {
        cerr << "Failed to open stats/obstacleInfo.json" << endl;
        return 1;
    }
    json obstacleInfo;
    file >> obstacleInfo;

    ofstream luaFile("test_obstacles.lua");
    if (!luaFile.is_open()) {
        cerr << "Failed to create test_obstacles.lua" << endl;
        return 1;
    }
    AddHeader(luaFile);

    luaFile << "local instance = homm3lua.new(homm3lua.FORMAT_ROE,homm3lua.SIZE_LARGE)\n";
    luaFile << "instance:name('Obstacle Test')\n";
    luaFile << "instance:description('Map to test all obstacles')\n";
    luaFile << "instance:difficulty(homm3lua.DIFFICULTY_EASY)\n\n";

    luaFile << "instance:terrain(homm3lua.TERRAIN_GRASS)\n";
    luaFile << "instance:player(homm3lua.PLAYER_1)\n";
    luaFile << "instance:player(homm3lua.PLAYER_2)\n";

    // Place obstacles with spacing
    int currentX = 1;
    int currentY = 2;
    int maxH     = 0;

    for (const auto &item : obstacleInfo) {
        string name    = item["name"].get<string>();
        string sizeStr = item["size"].get<string>();
        int commaPos   = sizeStr.find(',');
        int w          = stoi(sizeStr.substr(0, commaPos));
        int h          = stoi(sizeStr.substr(commaPos + 1));

        if (currentX + w + 2 > 105) { // +2 for border
            currentX = 1;
            currentY += maxH + 1; // +2 for border
            maxH = 0;
        }

        // HoMM3 objects are anchored at the bottom-right tile of their bounding box
        int3 anchorPos(currentX + w - 1, currentY + h - 1, 0);
        AddObstacle(luaFile, name, anchorPos);

        currentX += w + 1;
        maxH = max(maxH, h);
    }

    string homeDir      = getenv("HOME");
    string saveLocation = homeDir + "/.local/share/vcmi/Maps/test_obstacles.h3m";
    luaFile << "instance:write('" + saveLocation + "')\n";

    luaFile.close();
    cout << "Lua script created successfully at test_obstacles.lua" << endl;

    return 0;
}
