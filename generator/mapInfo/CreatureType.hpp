#pragma once

#include "../global/Global.hpp"

enum class CreatureType {
    PIKEMAN = 0,
    HALBERDIER,
    ARCHER,
    MARKSMAN,
    GRIFFIN,
    ROYAL_GRIFFIN,
    SWORDSMAN,
    CRUSADER,
    MONK,
    ZEALOT,
    CAVALIER,
    CHAMPION,
    ANGEL,
    ARCHANGEL,
    CENTAUR,
    CENTAUR_CAPTAIN,
    DWARF,
    BATTLE_DWARF,
    WOOD_ELF,
    GRAND_ELF,
    PEGASUS,
    SILVER_PEGASUS,
    DENDROID_GUARD,
    DENDROID_SOLDIER,
    UNICORN,
    WAR_UNICORN,
    GREEN_DRAGON,
    GOLD_DRAGON,
    GREMLIN,
    MASTER_GREMLIN,
    STONE_GARGOYLE,
    OBSIDIAN_GARGOYLE,
    STONE_GOLEM,
    IRON_GOLEM,
    MAGE,
    ARCH_MAGE,
    GENIE,
    MASTER_GENIE,
    NAGA,
    NAGA_QUEEN,
    GIANT,
    TITAN,
    IMP,
    FAMILIAR,
    GOG,
    MAGOG,
    HELL_HOUND,
    CERBERUS,
    DEMON,
    HORNED_DEMON,
    PIT_FIEND,
    PIT_LORD,
    EFREETI,
    EFREET_SULTAN,
    DEVIL,
    ARCH_DEVIL,
    SKELETON,
    SKELETON_WARRIOR,
    WALKING_DEAD,
    ZOMBIE,
    WIGHT,
    WRAITH,
    VAMPIRE,
    VAMPIRE_LORD,
    LICH,
    POWER_LICH,
    BLACK_KNIGHT,
    DREAD_KNIGHT,
    BONE_DRAGON,
    GHOST_DRAGON,
    TROGLODYTE,
    INFERNAL_TROGLODYTE,
    HARPY,
    HARPY_HAG,
    BEHOLDER,
    EVIL_EYE,
    MEDUSA,
    MEDUSA_QUEEN,
    MINOTAUR,
    MINOTAUR_KING,
    MANTICORE,
    SCORPICORE,
    RED_DRAGON,
    BLACK_DRAGON,
    GOBLIN,
    HOBGOBLIN,
    WOLF_RIDER,
    WOLF_RAIDER,
    ORC,
    ORC_CHIEFTAIN,
    OGRE,
    OGRE_MAGE,
    ROC,
    THUNDERBIRD,
    CYCLOPS,
    CYCLOPS_KING,
    BEHEMOTH,
    ANCIENT_BEHEMOTH,
    GNOLL,
    GNOLL_MARAUDER,
    LIZARDMAN,
    LIZARD_WARRIOR,
    GORGON,
    MIGHTY_GORGON,
    SERPENT_FLY,
    DRAGON_FLY,
    BASILISK,
    GREATER_BASILISK,
    WYVERN,
    WYVERN_MONARCH,
    HYDRA,
    CHAOS_HYDRA,
    AIR_ELEMENTAL,
    EARTH_ELEMENTAL,
    FIRE_ELEMENTAL,
    WATER_ELEMENTAL,
    GOLD_GOLEM,
    DIAMOND_GOLEM,
    PIXIE,
    SPRITE,
    PSYCHIC_ELEMENTAL,
    MAGIC_ELEMENTAL,
    ICE_ELEMENTAL,
    MAGMA_ELEMENTAL,
    STORM_ELEMENTAL,
    ENERGY_ELEMENTAL,
    FIREBIRD,
    PHOENIX,
    AZURE_DRAGON,
    CRYSTAL_DRAGON,
    FAERIE_DRAGON,
    RUST_DRAGON,
    ENCHANTER,
    SHARPSHOOTER,
    HALFLING,
    PEASANT,
    BOAR,
    MUMMY,
    NOMAD,
    ROGUE,
    TROLL,
    CATAPULT,
    BALLISTA,
    FIRST_AID_TENT,
    AMMO_CART,
    ARROW_TOWER,
    CREATURE_UNKNOWN
};

inline string creatureTypeToString(CreatureType creatureType) {
    switch (creatureType) {
    case CreatureType::PIKEMAN:
        return "Pikeman";
    case CreatureType::HALBERDIER:
        return "Halberdier";
    case CreatureType::ARCHER:
        return "Archer";
    case CreatureType::MARKSMAN:
        return "Marksman";
    case CreatureType::GRIFFIN:
        return "Griffin";
    case CreatureType::ROYAL_GRIFFIN:
        return "Royal Griffin";
    case CreatureType::SWORDSMAN:
        return "Swordsman";
    case CreatureType::CRUSADER:
        return "Crusader";
    case CreatureType::MONK:
        return "Monk";
    case CreatureType::ZEALOT:
        return "Zealot";
    case CreatureType::CAVALIER:
        return "Cavalier";
    case CreatureType::CHAMPION:
        return "Champion";
    case CreatureType::ANGEL:
        return "Angel";
    case CreatureType::ARCHANGEL:
        return "Archangel";
    case CreatureType::CENTAUR:
        return "Centaur";
    case CreatureType::CENTAUR_CAPTAIN:
        return "Centaur Captain";
    case CreatureType::DWARF:
        return "Dwarf";
    case CreatureType::BATTLE_DWARF:
        return "Battle Dwarf";
    case CreatureType::WOOD_ELF:
        return "Wood Elf";
    case CreatureType::GRAND_ELF:
        return "Grand Elf";
    case CreatureType::PEGASUS:
        return "Pegasus";
    case CreatureType::SILVER_PEGASUS:
        return "Silver Pegasus";
    case CreatureType::DENDROID_GUARD:
        return "Dendroid Guard";
    case CreatureType::DENDROID_SOLDIER:
        return "Dendroid Soldier";
    case CreatureType::UNICORN:
        return "Unicorn";
    case CreatureType::WAR_UNICORN:
        return "War Unicorn";
    case CreatureType::GREEN_DRAGON:
        return "Green Dragon";
    case CreatureType::GOLD_DRAGON:
        return "Gold Dragon";
    case CreatureType::GREMLIN:
        return "Gremlin";
    case CreatureType::MASTER_GREMLIN:
        return "Master Gremlin";
    case CreatureType::STONE_GARGOYLE:
        return "Stone Gargoyle";
    case CreatureType::OBSIDIAN_GARGOYLE:
        return "Obsidian Gargoyle";
    case CreatureType::STONE_GOLEM:
        return "Stone Golem";
    case CreatureType::IRON_GOLEM:
        return "Iron Golem";
    case CreatureType::MAGE:
        return "Mage";
    case CreatureType::ARCH_MAGE:
        return "Arch Mage";
    case CreatureType::GENIE:
        return "Genie";
    case CreatureType::MASTER_GENIE:
        return "Master Genie";
    case CreatureType::NAGA:
        return "Naga";
    case CreatureType::NAGA_QUEEN:
        return "Naga Queen";
    case CreatureType::GIANT:
        return "Giant";
    case CreatureType::TITAN:
        return "Titan";
    case CreatureType::IMP:
        return "Imp";
    case CreatureType::FAMILIAR:
        return "Familiar";
    case CreatureType::GOG:
        return "Gog";
    case CreatureType::MAGOG:
        return "Magog";
    case CreatureType::HELL_HOUND:
        return "Hell Hound";
    case CreatureType::CERBERUS:
        return "Cerberus";
    case CreatureType::DEMON:
        return "Demon";
    case CreatureType::HORNED_DEMON:
        return "Horned Demon";
    case CreatureType::PIT_FIEND:
        return "Pit Fiend";
    case CreatureType::PIT_LORD:
        return "Pit Lord";
    case CreatureType::EFREETI:
        return "Efreeti";
    case CreatureType::EFREET_SULTAN:
        return "Efreet Sultan";
    case CreatureType::DEVIL:
        return "Devil";
    case CreatureType::ARCH_DEVIL:
        return "Arch Devil";
    case CreatureType::SKELETON:
        return "Skeleton";
    case CreatureType::SKELETON_WARRIOR:
        return "Skeleton Warrior";
    case CreatureType::WALKING_DEAD:
        return "Walking Dead";
    case CreatureType::ZOMBIE:
        return "Zombie";
    case CreatureType::WIGHT:
        return "Wight";
    case CreatureType::WRAITH:
        return "Wraith";
    case CreatureType::VAMPIRE:
        return "Vampire";
    case CreatureType::VAMPIRE_LORD:
        return "Vampire Lord";
    case CreatureType::LICH:
        return "Lich";
    case CreatureType::POWER_LICH:
        return "Power Lich";
    case CreatureType::BLACK_KNIGHT:
        return "Black Knight";
    case CreatureType::DREAD_KNIGHT:
        return "Dread Knight";
    case CreatureType::BONE_DRAGON:
        return "Bone Dragon";
    case CreatureType::GHOST_DRAGON:
        return "Ghost Dragon";
    case CreatureType::TROGLODYTE:
        return "Troglodyte";
    case CreatureType::INFERNAL_TROGLODYTE:
        return "Infernal Troglodyte";
    case CreatureType::HARPY:
        return "Harpy";
    case CreatureType::HARPY_HAG:
        return "Harpy Hag";
    case CreatureType::BEHOLDER:
        return "Beholder";
    case CreatureType::EVIL_EYE:
        return "Evil Eye";
    case CreatureType::MEDUSA:
        return "Medusa";
    case CreatureType::MEDUSA_QUEEN:
        return "Medusa Queen";
    case CreatureType::MINOTAUR:
        return "Minotaur";
    case CreatureType::MINOTAUR_KING:
        return "Minotaur King";
    case CreatureType::MANTICORE:
        return "Manticore";
    case CreatureType::SCORPICORE:
        return "Scorpicore";
    case CreatureType::RED_DRAGON:
        return "Red Dragon";
    case CreatureType::BLACK_DRAGON:
        return "Black Dragon";
    case CreatureType::GOBLIN:
        return "Goblin";
    case CreatureType::HOBGOBLIN:
        return "Hobgoblin";
    case CreatureType::WOLF_RIDER:
        return "Wolf Rider";
    case CreatureType::WOLF_RAIDER:
        return "Wolf Raider";
    case CreatureType::ORC:
        return "Orc";
    case CreatureType::ORC_CHIEFTAIN:
        return "Orc Chieftain";
    case CreatureType::OGRE:
        return "Ogre";
    case CreatureType::OGRE_MAGE:
        return "Ogre Mage";
    case CreatureType::ROC:
        return "Roc";
    case CreatureType::THUNDERBIRD:
        return "Thunderbird";
    case CreatureType::CYCLOPS:
        return "Cyclops";
    case CreatureType::CYCLOPS_KING:
        return "Cyclops King";
    case CreatureType::BEHEMOTH:
        return "Behemoth";
    case CreatureType::ANCIENT_BEHEMOTH:
        return "Ancient Behemoth";
    case CreatureType::GNOLL:
        return "Gnoll";
    case CreatureType::GNOLL_MARAUDER:
        return "Gnoll Marauder";
    case CreatureType::LIZARDMAN:
        return "Lizardman";
    case CreatureType::LIZARD_WARRIOR:
        return "Lizard Warrior";
    case CreatureType::GORGON:
        return "Gorgon";
    case CreatureType::MIGHTY_GORGON:
        return "Mighty Gorgon";
    case CreatureType::SERPENT_FLY:
        return "Serpent Fly";
    case CreatureType::DRAGON_FLY:
        return "Dragon Fly";
    case CreatureType::BASILISK:
        return "Basilisk";
    case CreatureType::GREATER_BASILISK:
        return "Greater Basilisk";
    case CreatureType::WYVERN:
        return "Wyvern";
    case CreatureType::WYVERN_MONARCH:
        return "Wyvern Monarch";
    case CreatureType::HYDRA:
        return "Hydra";
    case CreatureType::CHAOS_HYDRA:
        return "Chaos Hydra";
    case CreatureType::AIR_ELEMENTAL:
        return "Air Elemental";
    case CreatureType::EARTH_ELEMENTAL:
        return "Earth Elemental";
    case CreatureType::FIRE_ELEMENTAL:
        return "Fire Elemental";
    case CreatureType::WATER_ELEMENTAL:
        return "Water Elemental";
    case CreatureType::GOLD_GOLEM:
        return "Gold Golem";
    case CreatureType::DIAMOND_GOLEM:
        return "Diamond Golem";
    case CreatureType::PIXIE:
        return "Pixie";
    case CreatureType::SPRITE:
        return "Sprite";
    case CreatureType::PSYCHIC_ELEMENTAL:
        return "Psychic Elemental";
    case CreatureType::MAGIC_ELEMENTAL:
        return "Magic Elemental";
    case CreatureType::ICE_ELEMENTAL:
        return "Ice Elemental";
    case CreatureType::MAGMA_ELEMENTAL:
        return "Magma Elemental";
    case CreatureType::STORM_ELEMENTAL:
        return "Storm Elemental";
    case CreatureType::ENERGY_ELEMENTAL:
        return "Energy Elemental";
    case CreatureType::FIREBIRD:
        return "Firebird";
    case CreatureType::PHOENIX:
        return "Phoenix";
    case CreatureType::AZURE_DRAGON:
        return "Azure Dragon";
    case CreatureType::CRYSTAL_DRAGON:
        return "Crystal Dragon";
    case CreatureType::FAERIE_DRAGON:
        return "Faerie Dragon";
    case CreatureType::RUST_DRAGON:
        return "Rust Dragon";
    case CreatureType::ENCHANTER:
        return "Enchanter";
    case CreatureType::SHARPSHOOTER:
        return "Sharpshooter";
    case CreatureType::HALFLING:
        return "Halfling";
    case CreatureType::PEASANT:
        return "Peasant";
    case CreatureType::BOAR:
        return "Boar";
    case CreatureType::MUMMY:
        return "Mummy";
    case CreatureType::NOMAD:
        return "Nomad";
    case CreatureType::ROGUE:
        return "Rogue";
    case CreatureType::TROLL:
        return "Troll";
    case CreatureType::CATAPULT:
        return "Catapult";
    case CreatureType::BALLISTA:
        return "Ballista";
    case CreatureType::FIRST_AID_TENT:
        return "First Aid Tent";
    case CreatureType::AMMO_CART:
        return "Ammo Cart";
    case CreatureType::ARROW_TOWER:
        return "Arrow Tower";
    default:
        return "CREATURE_UNKNOWN";
    }
}

inline CreatureType stringToCreatureType(string creatureType) {
    if (creatureType == "Pikeman")
        return CreatureType::PIKEMAN;
    if (creatureType == "Halberdier")
        return CreatureType::HALBERDIER;
    if (creatureType == "Archer")
        return CreatureType::ARCHER;
    if (creatureType == "Marksman")
        return CreatureType::MARKSMAN;
    if (creatureType == "Griffin")
        return CreatureType::GRIFFIN;
    if (creatureType == "Royal Griffin")
        return CreatureType::ROYAL_GRIFFIN;
    if (creatureType == "Swordsman")
        return CreatureType::SWORDSMAN;
    if (creatureType == "Crusader")
        return CreatureType::CRUSADER;
    if (creatureType == "Monk")
        return CreatureType::MONK;
    if (creatureType == "Zealot")
        return CreatureType::ZEALOT;
    if (creatureType == "Cavalier")
        return CreatureType::CAVALIER;
    if (creatureType == "Champion")
        return CreatureType::CHAMPION;
    if (creatureType == "Angel")
        return CreatureType::ANGEL;
    if (creatureType == "Archangel")
        return CreatureType::ARCHANGEL;
    if (creatureType == "Centaur")
        return CreatureType::CENTAUR;
    if (creatureType == "Centaur Captain")
        return CreatureType::CENTAUR_CAPTAIN;
    if (creatureType == "Dwarf")
        return CreatureType::DWARF;
    if (creatureType == "Battle Dwarf")
        return CreatureType::BATTLE_DWARF;
    if (creatureType == "Wood Elf")
        return CreatureType::WOOD_ELF;
    if (creatureType == "Grand Elf")
        return CreatureType::GRAND_ELF;
    if (creatureType == "Pegasus")
        return CreatureType::PEGASUS;
    if (creatureType == "Silver Pegasus")
        return CreatureType::SILVER_PEGASUS;
    if (creatureType == "Dendroid Guard")
        return CreatureType::DENDROID_GUARD;
    if (creatureType == "Dendroid Soldier")
        return CreatureType::DENDROID_SOLDIER;
    if (creatureType == "Unicorn")
        return CreatureType::UNICORN;
    if (creatureType == "War Unicorn")
        return CreatureType::WAR_UNICORN;
    if (creatureType == "Green Dragon")
        return CreatureType::GREEN_DRAGON;
    if (creatureType == "Gold Dragon")
        return CreatureType::GOLD_DRAGON;
    if (creatureType == "Gremlin")
        return CreatureType::GREMLIN;
    if (creatureType == "Master Gremlin")
        return CreatureType::MASTER_GREMLIN;
    if (creatureType == "Stone Gargoyle")
        return CreatureType::STONE_GARGOYLE;
    if (creatureType == "Obsidian Gargoyle")
        return CreatureType::OBSIDIAN_GARGOYLE;
    if (creatureType == "Stone Golem")
        return CreatureType::STONE_GOLEM;
    if (creatureType == "Iron Golem")
        return CreatureType::IRON_GOLEM;
    if (creatureType == "Mage")
        return CreatureType::MAGE;
    if (creatureType == "Arch Mage")
        return CreatureType::ARCH_MAGE;
    if (creatureType == "Genie")
        return CreatureType::GENIE;
    if (creatureType == "Master Genie")
        return CreatureType::MASTER_GENIE;
    if (creatureType == "Naga")
        return CreatureType::NAGA;
    if (creatureType == "Naga Queen")
        return CreatureType::NAGA_QUEEN;
    if (creatureType == "Giant")
        return CreatureType::GIANT;
    if (creatureType == "Titan")
        return CreatureType::TITAN;
    if (creatureType == "Imp")
        return CreatureType::IMP;
    if (creatureType == "Familiar")
        return CreatureType::FAMILIAR;
    if (creatureType == "Gog")
        return CreatureType::GOG;
    if (creatureType == "Magog")
        return CreatureType::MAGOG;
    if (creatureType == "Hell Hound")
        return CreatureType::HELL_HOUND;
    if (creatureType == "Cerberus")
        return CreatureType::CERBERUS;
    if (creatureType == "Demon")
        return CreatureType::DEMON;
    if (creatureType == "Horned Demon")
        return CreatureType::HORNED_DEMON;
    if (creatureType == "Pit Fiend")
        return CreatureType::PIT_FIEND;
    if (creatureType == "Pit Lord")
        return CreatureType::PIT_LORD;
    if (creatureType == "Efreeti")
        return CreatureType::EFREETI;
    if (creatureType == "Efreet Sultan")
        return CreatureType::EFREET_SULTAN;
    if (creatureType == "Devil")
        return CreatureType::DEVIL;
    if (creatureType == "Arch Devil")
        return CreatureType::ARCH_DEVIL;
    if (creatureType == "Skeleton")
        return CreatureType::SKELETON;
    if (creatureType == "Skeleton Warrior")
        return CreatureType::SKELETON_WARRIOR;
    if (creatureType == "Walking Dead")
        return CreatureType::WALKING_DEAD;
    if (creatureType == "Zombie")
        return CreatureType::ZOMBIE;
    if (creatureType == "Wight")
        return CreatureType::WIGHT;
    if (creatureType == "Wraith")
        return CreatureType::WRAITH;
    if (creatureType == "Vampire")
        return CreatureType::VAMPIRE;
    if (creatureType == "Vampire Lord")
        return CreatureType::VAMPIRE_LORD;
    if (creatureType == "Lich")
        return CreatureType::LICH;
    if (creatureType == "Power Lich")
        return CreatureType::POWER_LICH;
    if (creatureType == "Black Knight")
        return CreatureType::BLACK_KNIGHT;
    if (creatureType == "Dread Knight")
        return CreatureType::DREAD_KNIGHT;
    if (creatureType == "Bone Dragon")
        return CreatureType::BONE_DRAGON;
    if (creatureType == "Ghost Dragon")
        return CreatureType::GHOST_DRAGON;
    if (creatureType == "Troglodyte")
        return CreatureType::TROGLODYTE;
    if (creatureType == "Infernal Troglodyte")
        return CreatureType::INFERNAL_TROGLODYTE;
    if (creatureType == "Harpy")
        return CreatureType::HARPY;
    if (creatureType == "Harpy Hag")
        return CreatureType::HARPY_HAG;
    if (creatureType == "Beholder")
        return CreatureType::BEHOLDER;
    if (creatureType == "Evil Eye")
        return CreatureType::EVIL_EYE;
    if (creatureType == "Medusa")
        return CreatureType::MEDUSA;
    if (creatureType == "Medusa Queen")
        return CreatureType::MEDUSA_QUEEN;
    if (creatureType == "Minotaur")
        return CreatureType::MINOTAUR;
    if (creatureType == "Minotaur King")
        return CreatureType::MINOTAUR_KING;
    if (creatureType == "Manticore")
        return CreatureType::MANTICORE;
    if (creatureType == "Scorpicore")
        return CreatureType::SCORPICORE;
    if (creatureType == "Red Dragon")
        return CreatureType::RED_DRAGON;
    if (creatureType == "Black Dragon")
        return CreatureType::BLACK_DRAGON;
    if (creatureType == "Goblin")
        return CreatureType::GOBLIN;
    if (creatureType == "Hobgoblin")
        return CreatureType::HOBGOBLIN;
    if (creatureType == "Wolf Rider")
        return CreatureType::WOLF_RIDER;
    if (creatureType == "Wolf Raider")
        return CreatureType::WOLF_RAIDER;
    if (creatureType == "Orc")
        return CreatureType::ORC;
    if (creatureType == "Orc Chieftain")
        return CreatureType::ORC_CHIEFTAIN;
    if (creatureType == "Ogre")
        return CreatureType::OGRE;
    if (creatureType == "Ogre Mage")
        return CreatureType::OGRE_MAGE;
    if (creatureType == "Roc")
        return CreatureType::ROC;
    if (creatureType == "Thunderbird")
        return CreatureType::THUNDERBIRD;
    if (creatureType == "Cyclops")
        return CreatureType::CYCLOPS;
    if (creatureType == "Cyclops King")
        return CreatureType::CYCLOPS_KING;
    if (creatureType == "Behemoth")
        return CreatureType::BEHEMOTH;
    if (creatureType == "Ancient Behemoth")
        return CreatureType::ANCIENT_BEHEMOTH;
    if (creatureType == "Gnoll")
        return CreatureType::GNOLL;
    if (creatureType == "Gnoll Marauder")
        return CreatureType::GNOLL_MARAUDER;
    if (creatureType == "Lizardman")
        return CreatureType::LIZARDMAN;
    if (creatureType == "Lizard Warrior")
        return CreatureType::LIZARD_WARRIOR;
    if (creatureType == "Gorgon")
        return CreatureType::GORGON;
    if (creatureType == "Mighty Gorgon")
        return CreatureType::MIGHTY_GORGON;
    if (creatureType == "Serpent Fly")
        return CreatureType::SERPENT_FLY;
    if (creatureType == "Dragon Fly")
        return CreatureType::DRAGON_FLY;
    if (creatureType == "Basilisk")
        return CreatureType::BASILISK;
    if (creatureType == "Greater Basilisk")
        return CreatureType::GREATER_BASILISK;
    if (creatureType == "Wyvern")
        return CreatureType::WYVERN;
    if (creatureType == "Wyvern Monarch")
        return CreatureType::WYVERN_MONARCH;
    if (creatureType == "Hydra")
        return CreatureType::HYDRA;
    if (creatureType == "Chaos Hydra")
        return CreatureType::CHAOS_HYDRA;
    if (creatureType == "Air Elemental")
        return CreatureType::AIR_ELEMENTAL;
    if (creatureType == "Earth Elemental")
        return CreatureType::EARTH_ELEMENTAL;
    if (creatureType == "Fire Elemental")
        return CreatureType::FIRE_ELEMENTAL;
    if (creatureType == "Water Elemental")
        return CreatureType::WATER_ELEMENTAL;
    if (creatureType == "Gold Golem")
        return CreatureType::GOLD_GOLEM;
    if (creatureType == "Diamond Golem")
        return CreatureType::DIAMOND_GOLEM;
    if (creatureType == "Pixie")
        return CreatureType::PIXIE;
    if (creatureType == "Sprite")
        return CreatureType::SPRITE;
    if (creatureType == "Psychic Elemental")
        return CreatureType::PSYCHIC_ELEMENTAL;
    if (creatureType == "Magic Elemental")
        return CreatureType::MAGIC_ELEMENTAL;
    if (creatureType == "Ice Elemental")
        return CreatureType::ICE_ELEMENTAL;
    if (creatureType == "Magma Elemental")
        return CreatureType::MAGMA_ELEMENTAL;
    if (creatureType == "Storm Elemental")
        return CreatureType::STORM_ELEMENTAL;
    if (creatureType == "Energy Elemental")
        return CreatureType::ENERGY_ELEMENTAL;
    if (creatureType == "Firebird")
        return CreatureType::FIREBIRD;
    if (creatureType == "Phoenix")
        return CreatureType::PHOENIX;
    if (creatureType == "Azure Dragon")
        return CreatureType::AZURE_DRAGON;
    if (creatureType == "Crystal Dragon")
        return CreatureType::CRYSTAL_DRAGON;
    if (creatureType == "Faerie Dragon")
        return CreatureType::FAERIE_DRAGON;
    if (creatureType == "Rust Dragon")
        return CreatureType::RUST_DRAGON;
    if (creatureType == "Enchanter")
        return CreatureType::ENCHANTER;
    if (creatureType == "Sharpshooter")
        return CreatureType::SHARPSHOOTER;
    if (creatureType == "Halfling")
        return CreatureType::HALFLING;
    if (creatureType == "Peasant")
        return CreatureType::PEASANT;
    if (creatureType == "Boar")
        return CreatureType::BOAR;
    if (creatureType == "Mummy")
        return CreatureType::MUMMY;
    if (creatureType == "Nomad")
        return CreatureType::NOMAD;
    if (creatureType == "Rogue")
        return CreatureType::ROGUE;
    if (creatureType == "Troll")
        return CreatureType::TROLL;
    if (creatureType == "Catapult")
        return CreatureType::CATAPULT;
    if (creatureType == "Ballista")
        return CreatureType::BALLISTA;
    if (creatureType == "First Aid Tent")
        return CreatureType::FIRST_AID_TENT;
    if (creatureType == "Ammo Cart")
        return CreatureType::AMMO_CART;
    if (creatureType == "Arrow Tower")
        return CreatureType::ARROW_TOWER;
    return CreatureType::CREATURE_UNKNOWN;
}
