#include <bits/stdc++.h>
#include <algorithm>

#include "./luaUtils/lua_helpers.hpp"

#include "./layoutInfo/LayoutInfo.h"


using json = nlohmann::json;

LayoutInfo parseLayout(const json& layout) {
    LayoutInfo layoutInfo;
    layoutInfo.deserialize(layout);

    return layoutInfo;
}

// void generateLuaScript(LayoutInfo layoutInfo) {
      
//     std::ofstream luaFile("generated_script.lua");
//     if (!luaFile.is_open()) {
//         std::cerr << "Failed to create Lua script file." << std::endl;
//         return;
//     }
//     AddHeader(luaFile);

//     luaFile << "local instance = homm3lua.new(homm3lua.FORMAT_ROE, homm3lua.SIZE_";

//     luaFile << encodeMapSize(templateInfo.getMapSize()) << ")" << "\n";

//     luaFile << "instance:name('" << templateInfo.getName() << "')\n";
//     luaFile << "instance:description('" << templateInfo.getDescription() << "')\n";
//     std::string difficulty = templateInfo.getDifficulty();
//     std::transform(difficulty.begin(), difficulty.end(), difficulty.begin(), ::toupper);
//     luaFile << "instance:difficulty(homm3lua.DIFFICULTY_" << difficulty << ")\n\n";

//     // Map map(&rng);
//     // map.generateMap(templateInfo);


//     // if (config.value("debug", false)) {
//     //     std::cerr << "Map generated\n";
//     //     map.print();
//     // }

//     // Finishing luaScript
//     std::string homeDir = getenv("HOME");

//     if(saveLocation == "") {
//         saveLocation = homeDir + "/.local/share/vcmi/Maps/test.h3m";
//     } 

//     luaFile << "instance:write('" + saveLocation + "')";
//     luaFile << "\n";
    
//     luaFile.close();
//     std::cout << "Lua script generated successfully." << std::endl;
// }

void execute_lua_script(const std::string& script_name) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    if (luaL_dofile(L, script_name.c_str())) {
        std::cerr << "Error executing Lua script '" << script_name 
                  << "': " << lua_tostring(L, -1) << std::endl;
        lua_close(L);
    }

    lua_close(L);
}

int main(int argc, char *argv[]){
    std::ifstream file("layout.json");
    if (!file.is_open()) {
        std::cerr << "Failed to open layout.json file." << std::endl;
        return 1;
    }

    json layout;
    try {
        file >> layout;
    } catch (const json::parse_error& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        return 1;
    }

    RNG rng;
    std::string saveLocation = "";    


    for(int i = 1; i < argc; i++) {
        if(strcmp(argv[i], "--seed") == 0) {
            int seed = atoi(argv[i+1]);
            rng.setSeed(seed);
        }
        if(strcmp(argv[i], "--location") == 0) {
            saveLocation = argv[i+1];
        }
    }
    
    
    LayoutInfo layoutInfo = parseLayout(layout);
    layoutInfo.printLayout();
    
    // generateLuaScript(config);

    // std::cout << "Executing Lua script...\n";
    // execute_lua_script("generated_script.lua");
    // std::cout << "Lua script executed successfully.\n";

    return 0;
}