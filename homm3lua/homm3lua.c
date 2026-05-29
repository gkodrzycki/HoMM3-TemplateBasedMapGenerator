#include "homm3lua.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// Glue between homm3tools and Lua.
// @module homm3lua

/// Creates a new map.
// @function    new
// @tparam      integer     format    Map format. See FORMAT_*
// @tparam      integer     size      Map size. See SIZE_*
// @treturn     homm3lua              Map instance.
static int new(lua_State *L) {
    const int format = luaL_checkinteger(L, 1);
    const int size   = luaL_checkinteger(L, 2);

    h3mlib_ctx_t *h3m = (h3mlib_ctx_t *)lua_newuserdata(L, sizeof(h3mlib_ctx_t));

    if (h3m_init_min(h3m, format, size))
        return luaL_error(L, "h3m_init_min");

    (*h3m)->h3m.bi.any.has_hero = 1;

    luaL_setmetatable(L, "homm3lua");

    return 1;
}

/// Map instance.
// @type homm3lua

/// Clean up map memory.
// @local
// @function    __gc
static int __gc(lua_State *L) {
    h3mlib_ctx_t *h3m = (h3mlib_ctx_t *)luaL_checkudata(L, 1, "homm3lua");

    h3m_exit(h3m);

    return 0;
}

/// Place an artifact.
// @function    artifact
// @tparam      integer         artifact    Artifact name. See ARTIFACT_*
// @tparam      homm3lua_xyz    xyz         Position in {x, y, z} format.
static int artifact(lua_State *L) {
    h3mlib_ctx_t *h3m = (h3mlib_ctx_t *)luaL_checkudata(L, 1, "homm3lua");

    const char *artifact = luaL_checkstring(L, 2);
    const h3mlua_xyz xyz = h3mlua_check_xyz(L, 3);

    int object = 0;

    if (h3m_object_add(*h3m, artifact, xyz.x, xyz.y, xyz.z, &object))
        return luaL_error(L, "h3m_object_add");
    if ((*h3m)->meta.od_entries[object].oa_type != META_OBJECT_ARTIFACT)
        return luaL_argerror(L, 2, "it's not an artifact");

    return 0;
}

/// Place a creature.
// @function    creature
// @tparam      integer         creature         Creature name. See CREATURE_*
// @tparam      homm3lua_xyz    xyz              Position in {x, y, z} format.
// @tparam      integer         quantity         Quantity. Set to 0 for random integer.
// @tparam      integer         disposition      Creatures disposition. See DISPOSITION_*
// @tparam      boolean         never_flees      Disallows creatures to flee.
// @tparam      boolean         does_not_grow    Disallows creatures to grow.
static int creature(lua_State *L) {
    h3mlib_ctx_t *h3m = (h3mlib_ctx_t *)luaL_checkudata(L, 1, "homm3lua");

    const char *creature    = luaL_checkstring(L, 2);
    const h3mlua_xyz xyz    = h3mlua_check_xyz(L, 3);
    const int quantity      = luaL_checkinteger(L, 4);
    const int disposition   = luaL_checkinteger(L, 5);
    const int never_flees   = lua_toboolean(L, 6);
    const int does_not_grow = lua_toboolean(L, 7);

    int object = 0;

    if (h3m_object_add(*h3m, creature, xyz.x, xyz.y, xyz.z, &object))
        return luaL_error(L, "h3m_object_add");
    if ((*h3m)->meta.od_entries[object].oa_type != META_OBJECT_MONSTER)
        return luaL_argerror(L, 2, "it's not a creature");
    if (h3m_object_set_quantitiy(*h3m, object, quantity))
        return luaL_error(L, "h3m_object_set_quantitiy");
    if (h3m_object_set_disposition(*h3m, object, disposition))
        return luaL_error(L, "h3m_object_set_disposition");
    if (h3m_object_set_never_flees(*h3m, object, never_flees))
        return luaL_error(L, "h3m_object_set_never_flees");
    if (h3m_object_set_does_not_grow(*h3m, object, does_not_grow))
        return luaL_error(L, "h3m_object_set_does_not_grow");

    return 0;
}

/// Set map description.
// @function    description
// @tparam      string         description    Map description.
static int description(lua_State *L) {
    h3mlib_ctx_t *h3m = (h3mlib_ctx_t *)luaL_checkudata(L, 1, "homm3lua");

    const char *description = luaL_checkstring(L, 2);

    h3m_desc_set(*h3m, description);

    return 0;
}

/// Set map difficulty.
// @function    difficulty
// @tparam      integer       difficulty    Map difficulty. See DIFFICULTY_*
static int difficulty(lua_State *L) {
    h3mlib_ctx_t *h3m = (h3mlib_ctx_t *)luaL_checkudata(L, 1, "homm3lua");

    const int difficulty = luaL_checkinteger(L, 2);

    h3m_difficulty_set(*h3m, difficulty);

    return 0;
}

/// Place a hero.
// @function    hero
// @tparam      string          hero      Hero name. See HERO_*
// @tparam      homm3lua_xyz    xyz       Position in {x, y, z} format.
// @tparam      integer         player    Player. See PLAYER_*
static int hero(lua_State *L) {
    h3mlib_ctx_t *h3m = (h3mlib_ctx_t *)luaL_checkudata(L, 1, "homm3lua");

    const int hero       = luaL_checkinteger(L, 2);
    const h3mlua_xyz xyz = h3mlua_check_xyz(L, 3);
    const int player     = luaL_checkinteger(L, 4);

    static const char *models[] = {
        "Knight",    "Cleric",      "Ranger",       "Druid",       "Alchemist",    "Wizard",
        "Demoniac",  "Heretic",     "Death Knight", "Necromancer", "Overlord",     "Warlock",
        "Barbarian", "Battle Mage", "Beastmaster",  "Witch",       "Planeswalker", "Elementalist"};

    int object = 0;

    if (h3m_object_add(*h3m, models[hero / 8], xyz.x, xyz.y, xyz.z, &object))
        return luaL_error(L, "h3m_object_add");
    if ((*h3m)->meta.od_entries[object].oa_type != META_OBJECT_HERO)
        return luaL_argerror(L, 2, "it's not a hero");
    if (h3m_object_set_subtype(*h3m, object, hero))
        return luaL_error(L, "h3m_object_set_subtype");
    if (h3m_object_set_owner(*h3m, object, player))
        return luaL_error(L, "h3m_object_set_owner");

    return 0;
}

/// Place a mine.
// @function    mine
// @tparam      string          mine      Mine name. See MINE_*
// @tparam      homm3lua_xyz    xyz       Position in {x, y, z} format.
// @tparam      integer         player    Owner. See OWNER_*
static int mine(lua_State *L) {
    h3mlib_ctx_t *h3m = (h3mlib_ctx_t *)luaL_checkudata(L, 1, "homm3lua");

    const char *mine     = luaL_checkstring(L, 2);
    const h3mlua_xyz xyz = h3mlua_check_xyz(L, 3);
    const int owner      = luaL_checkinteger(L, 4);

    int object = 0;

    if (h3m_object_add(*h3m, mine, xyz.x, xyz.y, xyz.z, &object))
        return luaL_error(L, "h3m_object_add");
    if ((*h3m)->meta.od_entries[object].oa_type != META_OBJECT_RESOURCE_GENERATOR)
        return luaL_argerror(L, 2, "it's not a mine");
    if (owner != -1 && h3m_object_set_owner(*h3m, object, owner))
        return luaL_error(L, "h3m_object_set_owner");

    return 0;
}

/// Set map name.
// @function    name
// @tparam      string    name    Map name.
static int name(lua_State *L) {
    h3mlib_ctx_t *h3m = (h3mlib_ctx_t *)luaL_checkudata(L, 1, "homm3lua");

    const char *name = luaL_checkstring(L, 2);

    h3m_name_set(*h3m, name);

    return 0;
}

/// Enables player.
// @function    player
// @tparam      integer    player    Player. See PLAYER_*
static int player(lua_State *L) {
    h3mlib_ctx_t *h3m = (h3mlib_ctx_t *)luaL_checkudata(L, 1, "homm3lua");

    const int player = luaL_checkinteger(L, 2);

    h3m_player_enable(*h3m, player);

    return 0;
}

/// Place an obstacle.
// @function    obstacle
// @tparam      string          obstacle    Obstacle name. No constants so far.
// @tparam      homm3lua_xyz    xyz         Position in {x, y, z} format.
static int obstacle(lua_State *L) {
    h3mlib_ctx_t *h3m = (h3mlib_ctx_t *)luaL_checkudata(L, 1, "homm3lua");

    const char *obstacle = luaL_checkstring(L, 2);
    const h3mlua_xyz xyz = h3mlua_check_xyz(L, 3);

    int object = 0;

    if (h3m_object_add(*h3m, obstacle, xyz.x, xyz.y, xyz.z, &object))
        return luaL_error(L, "h3m_object_add");

    return 0;
}

/// Place a resource.
// @function    resource
// @tparam      string          resource    Obstacle name. No constants so far.
// @tparam      homm3lua_xyz    xyz         Position in {x, y, z} format.
// @tparam      integer         quantity    Quantity. Set to 0 for random integer.
static int resource(lua_State *L) {
    h3mlib_ctx_t *h3m = (h3mlib_ctx_t *)luaL_checkudata(L, 1, "homm3lua");

    const char *resource = luaL_checkstring(L, 2);
    const h3mlua_xyz xyz = h3mlua_check_xyz(L, 3);
    const int quantity   = luaL_checkinteger(L, 4);

    int object = 0;

    if (h3m_object_add(*h3m, resource, xyz.x, xyz.y, xyz.z, &object))
        return luaL_error(L, "h3m_object_add");
    if ((*h3m)->meta.od_entries[object].oa_type != META_OBJECT_RESOURCE)
        return luaL_argerror(L, 2, "it's not a resource");
    if (h3m_object_set_quantitiy(*h3m, object, quantity))
        return luaL_error(L, "h3m_object_set_quantitiy");

    return 0;
}

/// Place a sign.
// @function    sign
// @tparam      string          message     Sign message.
// @tparam      homm3lua_xyz    xyz         Position in {x, y, z} format.
static int sign(lua_State *L) {
    h3mlib_ctx_t *h3m = (h3mlib_ctx_t *)luaL_checkudata(L, 1, "homm3lua");

    size_t size;
    const char *message  = luaL_checklstring(L, 2, &size);
    const h3mlua_xyz xyz = h3mlua_check_xyz(L, 3);

    int object = 0;

    if (h3m_object_add(*h3m, "Sign", xyz.x, xyz.y, xyz.z, &object))
        return luaL_error(L, "h3m_object_add");
    if (h3m_object_set_message(*h3m, object, message, (uint32_t)size))
        return luaL_error(L, "h3m_object_set_message");

    return 0;
}

/// Draws terrain.
// @function    terrain
// @tparam      integer|function    terrain    Either a constant or a generating function of
// signature (homm3lua_xyz) -> (terrain?, road?, river?). See TERRAIN_*
static int terrain(lua_State *L) {
    h3mlib_ctx_t *h3m = (h3mlib_ctx_t *)luaL_checkudata(L, 1, "homm3lua");

    switch (lua_type(L, 2)) {
    case LUA_TFUNCTION: {
        const int both  = (*h3m)->h3m.bi.any.has_two_levels;
        const int size  = (*h3m)->h3m.bi.any.map_size;
        const int size2 = size * size;

        uint8_t *rivers  = calloc((1 + both) * size2, sizeof(uint8_t));
        uint8_t *roads   = calloc((1 + both) * size2, sizeof(uint8_t));
        uint8_t *terrain = malloc((1 + both) * size2);

        if (h3m_terrain_get_all((*h3m), 0, terrain, size2))
            return luaL_error(L, "h3m_terrain_get_all");
        if (both && h3m_terrain_get_all((*h3m), 1, terrain, size2))
            return luaL_error(L, "h3m_terrain_get_all");

        for (int z = 0; z < 1 + both; ++z)
            for (int y = 0; y < size; ++y)
                for (int x = 0; x < size; ++x) {
                    const int index = H3M_2D_TO_1D(size, x, y, z);

                    lua_pushvalue(L, 2);
                    lua_pushinteger(L, x);
                    lua_pushinteger(L, y);
                    lua_pushinteger(L, z);
                    lua_call(L, 3, 3);

                    if (!lua_isnil(L, -1))
                        rivers[index] = luaL_checkinteger(L, -1);
                    if (!lua_isnil(L, -2))
                        roads[index] = luaL_checkinteger(L, -2);
                    if (!lua_isnil(L, -3))
                        terrain[index] = luaL_checkinteger(L, -3);

                    lua_pop(L, 3);
                }

        if (h3m_generate_tiles((*h3m), size, 0, terrain, roads, rivers))
            return luaL_error(L, "h3m_generate_tiles");
        if (both &&
            h3m_generate_tiles((*h3m), size, 1, terrain + size2, roads + size2, rivers + size2))
            return luaL_error(L, "h3m_generate_tiles");

        free(rivers);
        free(roads);
        free(terrain);

        break;
    }

    case LUA_TNUMBER: {
        const int terrain = luaL_checkinteger(L, 2);

        if (h3m_terrain_fill(*h3m, terrain))
            return luaL_error(L, "h3m_terrain_fill");
        break;
    }

    default:
        return luaL_argerror(L, 2, "terrain must be a function or an integer");
    }

    return 0;
}

/// Write text with given object.
// @function    text
// @tparam      string          text       Text.
// @tparam      homm3lua_xyz    xyz        Position in {x, y, z} format. Left top corner.
// @tparam      string          object     Object.
static int text(lua_State *L) {
    h3mlib_ctx_t *h3m = (h3mlib_ctx_t *)luaL_checkudata(L, 1, "homm3lua");

    const char *text     = luaL_checkstring(L, 2);
    const h3mlua_xyz xyz = h3mlua_check_xyz(L, 3);
    const char *object   = luaL_checkstring(L, 4);

    if (h3m_object_text(*h3m, object, xyz.x, xyz.y, xyz.z, text))
        return luaL_error(L, "h3m_object_text");

    return 0;
}

/// Place a town.
// @function    town
// @tparam      string         town            Town name. See TOWN_*
// @tparam      homm3lua_xyz   xyz             Position in {x, y, z} format. Entry is at {x - 2, y,
// z}.
// @tparam      integer        owner           Owner. See OWNER_*
// @tparam      boolean        is_main_town    Optional. Set town as player main one.
// @tparam      boolean        has_fort        Optional. Whether the town starts with a fort built.
//                                             Defaults to true. Pass false for an unfortified town.
static const char *get_town_def(const char *town_name, int has_fort) {
    if (strcmp(town_name, "Castle") == 0)
        return has_fort ? "avccasx0.def" : "avccast0.def";
    if (strcmp(town_name, "Rampart") == 0)
        return has_fort ? "avcramx0.def" : "avcramp0.def";
    if (strcmp(town_name, "Tower") == 0)
        return has_fort ? "avctowx0.def" : "avctowr0.def";
    if (strcmp(town_name, "Inferno") == 0)
        return has_fort ? "avcinfx0.def" : "avcinft0.def";
    if (strcmp(town_name, "Necropolis") == 0)
        return has_fort ? "avcnecx0.def" : "avcnecr0.def";
    if (strcmp(town_name, "Dungeon") == 0)
        return has_fort ? "avcdunx0.def" : "avcdung0.def";
    if (strcmp(town_name, "Fortress") == 0)
        return has_fort ? "avcftrx0.def" : "avcftrt0.def";
    if (strcmp(town_name, "Stronghold") == 0)
        return has_fort ? "avcstrx0.def" : "avcstro0.def";
    if (strcmp(town_name, "Random Town") == 0)
        return has_fort ? "avcranx0.def" : "avcrand0.def";
    return NULL;
}

static int town(lua_State *L) {
    h3mlib_ctx_t *h3m = (h3mlib_ctx_t *)luaL_checkudata(L, 1, "homm3lua");

    const char *town       = luaL_checkstring(L, 2);
    const h3mlua_xyz xyz   = h3mlua_check_xyz(L, 3);
    const int owner        = luaL_checkinteger(L, 4);
    const int is_main_town = lua_toboolean(L, 5);
    const int has_fort     = lua_isnoneornil(L, 6) ? 1 : lua_toboolean(L, 6);

    const char *def = get_town_def(town, has_fort);
    if (!def)
        return luaL_argerror(L, 2, "unknown town type");

    int object = 0;

    if (h3m_object_add_by_def(*h3m, def, xyz.x, xyz.y, xyz.z, &object))
        return luaL_error(L, "h3m_object_add_by_def");
    if ((*h3m)->meta.od_entries[object].oa_type != META_OBJECT_TOWN)
        return luaL_argerror(L, 2, "it's not a town");
    if (owner != -1 && h3m_object_set_owner(*h3m, object, owner))
        return luaL_error(L, "h3m_object_set_owner");
    if (h3m_object_set_has_fort(*h3m, object, has_fort))
        return luaL_error(L, "h3m_object_set_has_fort");
    if (is_main_town) {
        if (owner == -1)
            return luaL_error(L, "Neutral town cannot be a main one.");

        if (strcmp(town, "Castle") == 0)
            (*h3m)->h3m.players[owner]->roe.town_types = 0x01;
        if (strcmp(town, "Rampart") == 0)
            (*h3m)->h3m.players[owner]->roe.town_types = 0x02;
        if (strcmp(town, "Tower") == 0)
            (*h3m)->h3m.players[owner]->roe.town_types = 0x04;
        if (strcmp(town, "Inferno") == 0)
            (*h3m)->h3m.players[owner]->roe.town_types = 0x08;
        if (strcmp(town, "Necropolis") == 0)
            (*h3m)->h3m.players[owner]->roe.town_types = 0x10;
        if (strcmp(town, "Dungeon") == 0)
            (*h3m)->h3m.players[owner]->roe.town_types = 0x20;
        if (strcmp(town, "Fortress") == 0)
            (*h3m)->h3m.players[owner]->roe.town_types = 0x40;
        if (strcmp(town, "Stronghold") == 0)
            (*h3m)->h3m.players[owner]->roe.town_types = 0x80;
        if (strcmp(town, "Random Town") == 0)
            (*h3m)->h3m.players[owner]->roe.town_types = 0xFF;

        (*h3m)->meta.player_sizes[owner]                             = 11;
        (*h3m)->h3m.players[owner]->roe.unknown1                     = 0;
        (*h3m)->h3m.players[owner]->roe.has_main_town                = 1;
        (*h3m)->h3m.players[owner]->roe.u.e1.starting_town_xpos      = xyz.x - 2;
        (*h3m)->h3m.players[owner]->roe.u.e1.starting_town_ypos      = xyz.y;
        (*h3m)->h3m.players[owner]->roe.u.e1.starting_town_zpos      = xyz.z;
        (*h3m)->h3m.players[owner]->roe.u.e1.starting_hero_is_random = 1;
        (*h3m)->h3m.players[owner]->roe.u.e1.starting_hero_type      = 0xFF;
    }

    return 0;
}

/// Configures underground presence.
// @function    underground
// @tparam      boolean        has_two_levels    Allows underground.
static int underground(lua_State *L) {
    h3mlib_ctx_t *h3m = (h3mlib_ctx_t *)luaL_checkudata(L, 1, "homm3lua");

    const uint8_t has_two_levels = lua_toboolean(L, 2);

    (*h3m)->h3m.bi.any.has_two_levels = has_two_levels;

    return 0;
}

/// Export map to a file.
// @function    write
// @tparam      string     path    File path.
static int write(lua_State *L) {
    h3mlib_ctx_t *h3m = (h3mlib_ctx_t *)luaL_checkudata(L, 1, "homm3lua");

    const char *path = luaL_checkstring(L, 2);

    if (h3m_write(*h3m, path))
        return luaL_error(L, "h3m_write");
    // https://github.com/potmdehex/homm3tools/issues/31
    // Meanwhile, normal gzip is working:
    //   gzip path && mv path.gz path
    // if (h3m_compress(path, path))
    //   return luaL_error(L, "h3m_compress");

    return 0;
}

// Write a 32-bit little-endian value into a byte buffer at the given offset.
static void write_le32(uint8_t *buf, int offset, uint32_t val) {
    buf[offset + 0] = val & 0xFF;
    buf[offset + 1] = (val >> 8) & 0xFF;
    buf[offset + 2] = (val >> 16) & 0xFF;
    buf[offset + 3] = (val >> 24) & 0xFF;
}

/// Place a Pandora's Box with optional content.
// @function    pandora
// @tparam      homm3lua_xyz    xyz       Position in {x, y, z} format.
// @tparam      table           content   Optional table with content fields:
//   gold, experience, spell_points, morale, luck,
//   attack, defense, spell_power, knowledge.
//
// Binary layout of the default Pandora's Box body (55 bytes, all-zero baseline):
//   [0]    has_guardians  (uint8)
//   [1-4]  experience     (uint32 LE)
//   [5-8]  spell_points   (uint32 LE)
//   [9]    morale         (int8)
//   [10]   luck           (int8)
//   [11-14] wood          (uint32 LE)
//   [15-18] mercury       (uint32 LE)
//   [19-22] ore           (uint32 LE)
//   [23-26] sulfur        (uint32 LE)
//   [27-30] crystal       (uint32 LE)
//   [31-34] gems          (uint32 LE)
//   [35-38] gold          (uint32 LE)
//   [39]   attack         (uint8)
//   [40]   defense        (uint8)
//   [41]   spell_power    (uint8)
//   [42]   knowledge      (uint8)
//   [43]   secondary_skills_count (uint8)
//   [44]   artifacts_count        (uint8)
//   [45]   spells_count           (uint8)
//   [46]   creatures_count        (uint8)
//   [47-54] unknown1[8]
static int pandora(lua_State *L) {
    h3mlib_ctx_t *h3m = (h3mlib_ctx_t *)luaL_checkudata(L, 1, "homm3lua");

    const h3mlua_xyz xyz = h3mlua_check_xyz(L, 2);

    int object = 0;
    if (h3m_object_add(*h3m, "Pandora's Box", xyz.x, xyz.y, xyz.z, &object))
        return luaL_error(L, "h3m_object_add: Pandora's Box");

    if ((*h3m)->meta.od_entries[object].oa_type != META_OBJECT_PANDORAS_BOX)
        return luaL_error(L, "internal error: expected PANDORAS_BOX type");

    uint8_t *body = (uint8_t *)(*h3m)->h3m.od.entries[object].body;

    if (lua_istable(L, 3)) {
        lua_getfield(L, 3, "gold");
        if (!lua_isnil(L, -1))
            write_le32(body, 35, (uint32_t)luaL_checkinteger(L, -1));
        lua_pop(L, 1);

        lua_getfield(L, 3, "experience");
        if (!lua_isnil(L, -1))
            write_le32(body, 1, (uint32_t)luaL_checkinteger(L, -1));
        lua_pop(L, 1);

        lua_getfield(L, 3, "spell_points");
        if (!lua_isnil(L, -1))
            write_le32(body, 5, (uint32_t)luaL_checkinteger(L, -1));
        lua_pop(L, 1);

        lua_getfield(L, 3, "morale");
        if (!lua_isnil(L, -1))
            body[9] = (uint8_t)(luaL_checkinteger(L, -1) & 0xFF);
        lua_pop(L, 1);

        lua_getfield(L, 3, "luck");
        if (!lua_isnil(L, -1))
            body[10] = (uint8_t)(luaL_checkinteger(L, -1) & 0xFF);
        lua_pop(L, 1);

        lua_getfield(L, 3, "attack");
        if (!lua_isnil(L, -1))
            body[39] = (uint8_t)(luaL_checkinteger(L, -1) & 0xFF);
        lua_pop(L, 1);

        lua_getfield(L, 3, "defense");
        if (!lua_isnil(L, -1))
            body[40] = (uint8_t)(luaL_checkinteger(L, -1) & 0xFF);
        lua_pop(L, 1);

        lua_getfield(L, 3, "spell_power");
        if (!lua_isnil(L, -1))
            body[41] = (uint8_t)(luaL_checkinteger(L, -1) & 0xFF);
        lua_pop(L, 1);

        lua_getfield(L, 3, "knowledge");
        if (!lua_isnil(L, -1))
            body[42] = (uint8_t)(luaL_checkinteger(L, -1) & 0xFF);
        lua_pop(L, 1);

        /* ---- artifacts: array of artifact name strings or numeric IDs ---- */
        int art_ids[64];
        int art_count = 0;
        lua_getfield(L, 3, "artifacts");
        if (!lua_isnil(L, -1) && lua_istable(L, -1)) {
            extern int object_names_hash(const char *key);
            lua_len(L, -1);
            int na = (int)lua_tointeger(L, -1);
            lua_pop(L, 1);
            if (na > 64)
                na = 64;
            for (int i = 0; i < na; ++i) {
                lua_rawgeti(L, -1, i + 1);
                if (lua_isstring(L, -1)) {
                    const char *aname = lua_tostring(L, -1);
                    char fixed[68]    = {0};
                    size_t alen       = strlen(aname);
                    /* h3mlib expects def name: append '0' if name ends in a letter */
                    if (alen > 0 && isalpha((unsigned char)aname[alen - 1]))
                        snprintf(fixed, sizeof(fixed), "%s0", aname);
                    else
                        snprintf(fixed, sizeof(fixed), "%s", aname);
                    int aid = object_names_hash(fixed);
                    if (aid >= 0)
                        art_ids[art_count++] = aid;
                } else if (lua_isinteger(L, -1)) {
                    art_ids[art_count++] = (int)lua_tointeger(L, -1);
                }
                lua_pop(L, 1);
            }
        }
        lua_pop(L, 1);

        /* ---- creatures: array of {name='X', quantity=N} tables ---- */
        int cr_types[64];
        int cr_qtys[64];
        int cr_count = 0;
        lua_getfield(L, 3, "creatures");
        if (!lua_isnil(L, -1) && lua_istable(L, -1)) {
            extern int creature_names_hash(const char *);
            lua_len(L, -1);
            int nc = (int)lua_tointeger(L, -1);
            lua_pop(L, 1);
            if (nc > 64)
                nc = 64;
            for (int i = 0; i < nc; ++i) {
                lua_rawgeti(L, -1, i + 1);
                if (!lua_isnil(L, -1) && lua_istable(L, -1)) {
                    lua_getfield(L, -1, "name");
                    const char *cname = lua_isstring(L, -1) ? lua_tostring(L, -1) : NULL;
                    lua_pop(L, 1);
                    lua_getfield(L, -1, "quantity");
                    int qty = lua_isinteger(L, -1) ? (int)lua_tointeger(L, -1) : 1;
                    lua_pop(L, 1);
                    if (cname) {
                        int cid = creature_names_hash(cname);
                        if (cid >= 0) {
                            cr_types[cr_count] = cid;
                            cr_qtys[cr_count]  = qty;
                            cr_count++;
                        }
                    }
                }
                lua_pop(L, 1);
            }
        }
        lua_pop(L, 1);

        /* ---- rebuild binary body when artifacts or creatures are present ----
         *
         * Default body (55 bytes, all zeros, binary-compatible) layout:
         *   [0]    has_guardians
         *   [1..43] experience, spell_points, morale, luck, resources[7],
         *           primary_skills, secondary_skills_count=0
         *   [44]   artifacts_count  then: artifacts_count * (1 or 2) bytes
         *   [?]    spells_count     then: spells_count bytes
         *   [?]    creatures_count  then: creatures_count * (3 or 4) bytes
         *   [?]    unknown1[8]
         * ROE uses 1-byte artifact IDs and 3-byte creature slots;
         * AB/SOD uses 2-byte artifact IDs and 4-byte creature slots.
         */
        if (art_count > 0 || cr_count > 0) {
            const int is_roe      = (int)((*h3m)->h3m.format == H3M_FORMAT_ROE);
            const int art_item_sz = is_roe ? 1 : 2;
            const int cr_slot_sz  = is_roe ? 3 : 4;
            const size_t prefix   = 44; /* bytes [0..43] are pure scalars */
            size_t new_size       = prefix + 1 + (size_t)art_count * (size_t)art_item_sz +
                              1 /* spells_count = 0 */
                              + 1 + (size_t)cr_count * (size_t)cr_slot_sz + 8; /* unknown1[8] */

            uint8_t *nb = (uint8_t *)calloc(new_size, 1);
            if (!nb)
                return luaL_error(L, "pandora: out of memory");

            memcpy(nb, body, prefix);
            size_t pos = prefix;

            /* artifacts */
            nb[pos++] = (uint8_t)art_count;
            for (int i = 0; i < art_count; ++i) {
                nb[pos++] = (uint8_t)(art_ids[i] & 0xFF);
                if (!is_roe)
                    nb[pos++] = (uint8_t)((art_ids[i] >> 8) & 0xFF);
            }

            /* spells: none */
            nb[pos++] = 0;

            /* creatures */
            nb[pos++] = (uint8_t)cr_count;
            for (int i = 0; i < cr_count; ++i) {
                if (is_roe) {
                    nb[pos++] = (uint8_t)(cr_types[i] & 0xFF);
                } else {
                    nb[pos++] = (uint8_t)(cr_types[i] & 0xFF);
                    nb[pos++] = (uint8_t)((cr_types[i] >> 8) & 0xFF);
                }
                nb[pos++] = (uint8_t)(cr_qtys[i] & 0xFF);
                nb[pos++] = (uint8_t)((cr_qtys[i] >> 8) & 0xFF);
            }
            /* unknown1[8] already zeroed by calloc */

            free((*h3m)->h3m.od.entries[object].body);
            (*h3m)->h3m.od.entries[object].body       = nb;
            (*h3m)->meta.od_entries[object].body_size = new_size;
        }
    }

    return 0;
}

// -----------------------------------------------------------------------------

static const struct luaL_Reg h3mlua_instance[] = {{"__gc", __gc},
                                                  {"artifact", artifact},
                                                  {"creature", creature},
                                                  {"description", description},
                                                  {"difficulty", difficulty},
                                                  {"hero", hero},
                                                  {"mine", mine},
                                                  {"name", name},
                                                  {"obstacle", obstacle},
                                                  {"pandora", pandora},
                                                  {"player", player},
                                                  {"resource", resource},
                                                  {"sign", sign},
                                                  {"terrain", terrain},
                                                  {"text", text},
                                                  {"town", town},
                                                  {"underground", underground},
                                                  {"write", write},
                                                  {NULL, NULL}};

static const struct luaL_Reg h3mlua[] = {{"new", new}, {NULL, NULL}};

// -----------------------------------------------------------------------------

int luaopen_homm3lua(lua_State *L) {
    luaL_newmetatable(L, "homm3lua");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, h3mlua_instance, 0);
    luaL_newlib(L, h3mlua);
    h3mlua_constants(L);

    return 1;
}
