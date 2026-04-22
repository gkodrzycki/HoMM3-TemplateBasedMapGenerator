#include "./luaUtils/luaHelpers.hpp"

#include "./blueprintInfo/BlueprintInfo.hpp"
#include "./global/Global.hpp"
#include "./global/Random.hpp"
#include "./layoutInfo/LayoutInfo.hpp"
#include "./map/Map.hpp"

LayoutInfo parseLayout(const json &layout) {
    LayoutInfo layoutInfo;
    layoutInfo.deserialize(layout);

    return layoutInfo;
}

BlueprintInfo parseBlueprint(const json &blueprint, RNG &rng) {
    BlueprintInfo blueprintInfo(rng);
    blueprintInfo.deserialize(blueprint);

    return blueprintInfo;
}

void generateLuaScript(Map map, string &saveLocation) {
    LayoutInfo layoutInfo = map.getLayoutInfo();

    ofstream luaFile("generated_script.lua");
    if (!luaFile.is_open()) {
        cerr << "Failed to create Lua script file." << endl;
        return;
    }
    AddHeader(luaFile);

    luaFile << "local instance = homm3lua.new(homm3lua.FORMAT_ROE,homm3lua.SIZE_"
            << encodeMapSize(layoutInfo.getMapSize()) << ")\n";

    luaFile << "instance:name('" << layoutInfo.getName() << "')\n";
    luaFile << "instance:description('" << layoutInfo.getDescription() << "')\n";
    string difficulty = layoutInfo.getDifficulty();
    transform(difficulty.begin(), difficulty.end(), difficulty.begin(), ::toupper);
    luaFile << "instance:difficulty(homm3lua.DIFFICULTY_" << difficulty << ")\n\n";

    AddTerrain(luaFile);
    AddTerrainTiles(luaFile, map);

    AddTowns(luaFile, map);

    AddBorderObstacles(luaFile, map);

    AddRoads(luaFile, map);

    AddMonoliths(luaFile, map);

    AddMines(luaFile, map);

    AddCreatures(luaFile, map);

    AddTreasures(luaFile, map);

    // Finishing luaScript
    string homeDir = getenv("HOME");

    if (saveLocation == "") {
        saveLocation = homeDir + "/.local/share/vcmi/Maps/test.h3m";
    }

    luaFile << "instance:write('" + saveLocation + "')";
    luaFile << "\n";

    luaFile.close();
    cout << "Lua script generated successfully." << endl;
}

void executeLuaScript(const string &script_name) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    if (luaL_dofile(L, script_name.c_str())) {
        cerr << "Error executing Lua script '" << script_name << "': " << lua_tostring(L, -1)
             << endl;
        lua_close(L);
        return;
    }

    lua_close(L);
}

json readFile(string filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Failed to open " << filename << " file." << endl;
        return 1;
    }

    json jsonFile;
    try {
        file >> jsonFile;
    } catch (const json::parse_error &e) {
        cerr << "JSON parsing error: " << e.what() << endl;
        return 1;
    }

    return jsonFile;
}

int main(int argc, char *argv[]) {
    json _template = readFile("template.json");
    // json layout    = readFile("layout.json");
    // json blueprint = readFile("blueprint.json");

    RNG rng;
    string saveLocation = "";

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--seed") == 0) {
            int seed = atoi(argv[i + 1]);
            rng.setSeed(seed);
        }
        if (strcmp(argv[i], "--location") == 0) {
            saveLocation = argv[i + 1];
        }
    }

    LayoutInfo layoutInfo = parseLayout(_template);
    if (layoutInfo.getDebug() > 1)
        layoutInfo.printLayout();

    BlueprintInfo blueprintInfo = parseBlueprint(_template, rng);
    if (layoutInfo.getDebug() > 1)
        blueprintInfo.printBlueprint();

    Map map(rng, layoutInfo, blueprintInfo);
    map.generateMap();
    if (layoutInfo.getDebug() > 0) {
        cerr << "========== MAP ==========\n";
        map.printMap(layoutInfo.getDebug());
        cerr << "Map printed!\n";
    }

    generateLuaScript(map, saveLocation);

    cout << "Executing Lua script...\n";
    executeLuaScript("generated_script.lua");
    cout << "Lua script executed successfully and saved to generated_script.lua\n";

    return 0;
}
