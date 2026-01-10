#include "../generator/luaUtils/luaHelpers.hpp"

#include "../generator/global/Global.hpp"

static vector<string> loadCreatureNamesFromTraits(const string &path) {
    ifstream file(path);
    if (!file.is_open()) {
        throw runtime_error("Failed to open " + path);
    }

    json traits;
    file >> traits;
    if (!traits.is_array()) {
        throw runtime_error(path + " is not a JSON array");
    }

    vector<string> names;
    names.reserve(traits.size());
    for (const auto &entry : traits) {
        if (!entry.is_object())
            continue;

        const string singular = getOrDefault<string>(entry, "singular", "");
        if (singular.empty())
            continue;
        if (singular.rfind("NOT USED", 0) == 0)
            continue;

        names.push_back(singular);
    }

    return names;
}

void generateLuaScript(string &saveLocation, const string &crtraitsPath) {
    ofstream luaFile("units.lua");
    if (!luaFile.is_open()) {
        cerr << "Failed to create Lua script file." << endl;
        return;
    }
    AddHeader(luaFile);

    luaFile << "local instance = homm3lua.new(homm3lua.FORMAT_ROE,homm3lua.SIZE_SMALL)\n";

    luaFile << "instance:name('Units')\n";
    luaFile << "instance:description('Map with units')\n";
    luaFile << "instance:difficulty(homm3lua.DIFFICULTY_EASY)\n\n";

    AddTerrain(luaFile);

    constexpr int factionsCount    = 8;
    constexpr int unitsPerFaction  = 14;
    const vector<string> creatures = loadCreatureNamesFromTraits(crtraitsPath);

    const int xStart = 2;
    const int yStart = 2;
    const int xStep  = 2;
    const int yStep  = 2;

    int idx = 0;
    for (int faction = 0; faction < factionsCount; faction++) {
        const int y = yStart + faction * yStep;
        for (int i = 0; i < unitsPerFaction; i++) {
            const int x = xStart + i * xStep;
            auto creature =
                Creature(creatures[idx], int3(x, y, 0), 1, "COMPLIANT", true, true, "Creature");
            AddCreature(luaFile, creature);
            idx++;
        }
        luaFile << "\n";
    }

    // Finishing luaScript
    string homeDir = getenv("HOME");

    if (saveLocation == "") {
        saveLocation = homeDir + "/.local/share/vcmi/Maps/units.h3m";
    }

    luaFile << "instance:write('" + saveLocation + "')\n";

    luaFile.close();
    cout << "Lua script generated successfully." << endl;
}

void executeLuaScript(const string &script_name) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    if (luaL_dofile(L, script_name.c_str())) {
        cerr << "Error executing Lua script '" << script_name << "': " << lua_tostring(L, -1)
             << endl;
    }

    lua_close(L);
}

int main(int argc, char *argv[]) {
    string saveLocation = "";
    string crtraitsPath = "CRTRAITS.json";

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--location") == 0) {
            saveLocation = argv[i + 1];
        }
        if (strcmp(argv[i], "--crtraits") == 0) {
            crtraitsPath = argv[i + 1];
        }
    }

    generateLuaScript(saveLocation, crtraitsPath);

    cout << "Executing Lua script...\n";
    executeLuaScript("units.lua");
    cout << "Lua script executed successfully and saved to units.lua\n";
    return 0;
}
