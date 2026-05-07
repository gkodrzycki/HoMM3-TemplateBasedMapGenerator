#include "./luaUtils/luaHelpers.hpp"

#include "./global/Global.hpp"
#include "./global/Random.hpp"
#include "./map/Map.hpp"
#include "./templateInfo/TemplateInfo.hpp"

void generateLuaScript(Map map, string &saveLocation) {
    TemplateInfo templateInfo = map.getTemplateInfo();

    ofstream luaFile("generated_script.lua");
    if (!luaFile.is_open()) {
        cerr << "Failed to create Lua script file." << endl;
        return;
    }
    AddHeader(luaFile);

    luaFile << "local instance = homm3lua.new(homm3lua.FORMAT_ROE,homm3lua.SIZE_"
            << encodeMapSize(map.getWidth(), map.getHeight()) << ")\n";

    luaFile << "instance:name('" << templateInfo.getName() << "')\n";
    luaFile << "instance:description('" << templateInfo.getDescription() << "')\n";

    // TODO: think about better handling difficulty
    string difficulty = "easy";
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
    RNG rng;
    int debug           = 0;
    string saveLocation = "";
    string templateName = "JebusCross.json";

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--seed") == 0) {
            int seed = atoi(argv[i + 1]);
            rng.setSeed(seed);
        }
        if (strcmp(argv[i], "--location") == 0) {
            saveLocation = argv[i + 1];
        }
        if (strcmp(argv[i], "--debug") == 0) {
            debug = atoi(argv[i + 1]);
        }
        if (strcmp(argv[i], "--template") == 0) {
            templateName = argv[i + 1];
        }
    }

    json _template = readFile(templateName);

    // LayoutInfo layoutInfo = parseLayout(_template);
    // if (layoutInfo.getDebug() > 1)
    //     layoutInfo.printLayout();

    // BlueprintInfo blueprintInfo = parseBlueprint(_template, rng);
    // if (layoutInfo.getDebug() > 1)
    //     blueprintInfo.printBlueprint();
    TemplateInfo templateInfo(debug);
    templateInfo.deserialize(_template);
    if (templateInfo.getDebug() > 2)
        templateInfo.print();

    Map map(rng, templateInfo);
    map.generateMap();

    if (templateInfo.getDebug() > 0) {
        cerr << "========== MAP ==========\n";
        map.printMap(templateInfo.getDebug());
        cerr << "Map printed!\n";
    }

    generateLuaScript(map, saveLocation);

    cout << "Executing Lua script...\n";
    executeLuaScript("generated_script.lua");
    cout << "Lua script executed successfully and saved to generated_script.lua\n";

    return 0;
}
