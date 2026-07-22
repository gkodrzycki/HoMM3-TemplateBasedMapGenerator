# Layout-Based Map Generator for Heroes of Might & Magic III
This repository contains the implementation accompanying the master's thesis. A link to the published paper will be added here once available.

## Overview

Heroes of Might and Magic III map generation tools are limited, and many existing generators produce maps that are unbalanced or lack long-term replayability. This project implements a **layout-based random map generator** for HoMM3 that produces varied, playable, and balanced maps.

The generator is driven by a single **JSON template** that defines the map's structure: zones (with sizes, terrain types, and object budgets), connections between zones, and per-zone content parameters (resource richness, guard strength, treasure density). Templates can be authored by hand or converted from the original HoMM3 `.h3t` format using `scripts/h3t_to_json.py`. Procedural placement algorithms fill in towns, roads, mines, treasures, guards, and obstacles. The result is maps with predictable macro-structure for balance, combined with local randomness for replayability.

### Generation Pipeline

```
Template (.json — converted from .h3t or authored directly)
          │
          ▼
    Zone Generation          ← abstract grid, Simulated Annealing, weighted claiming
          │
          ▼
    Border Classification    ← inner / outer border tiles
          │
          ▼
    Noise Generation         ← terrain obstacles (FastNoiseLite)
          │
          ▼
    Town Placement
          │
          ▼
    Connection Generation    ← roads (Chiseled Paths) + portals (monoliths)
          │
          ▼
    Object Placement         ← mines, treasures, guards
          │
          ▼
    Reachability Correction
          │
          ▼
    Physical Obstacle Placement  ← multi-tile objects, biome filtering, exclusion spacing
          │
          ▼
    Serialization            ← C++ → Lua script → .h3m binary
```

## Setup

### 1. Init submodules

```bash
git submodule update --recursive --init
```

### 2. Install dependencies

Requires: `Lua 5.4`, `nlohmann-json`, `magic-enum`

On Ubuntu:

```bash
sudo apt-get install lua5.4 liblua5.4-dev libtbb-dev libsdl2-ttf-dev \
    qttools5-dev libsdl2-mixer-dev libsdl2-image-dev \
    nlohmann-json3-dev libmagicenum-dev
```

You may also need symlinks for Lua:

```bash
sudo ln -s /usr/include/lua5.4 /usr/include/lua
sudo ln -s "$(pkg-config --variable=libdir lua5.4)/liblua5.4.so" /usr/lib/liblua.so
```

### 3. Build

```bash
make        # builds Generator, Units, TestObstacles, and dist/homm3lua.so
```

---

## Usage

### Generating a map

```bash
./Generator [--template <path>] [--seed <number>] [--location <path>] [--debug <level>]
```

| Flag | Default | Description |
|---|---|---|
| `--template` | `JebusCross.json` | Path to the JSON template file |
| `--seed` | random | Integer seed for reproducible generation |
| `--location` | `~/.local/share/vcmi/Maps/test.h3m` | Output path for the `.h3m` file |
| `--debug` | `0` | Debug verbosity: `1` prints each generation stage to stderr; `2` also prints the full tile map |

The generator writes a `generated_script.lua` alongside the binary and then executes it via Lua to produce the final `.h3m` file.

### Converting a template from .h3t to JSON

The original HoMM3 template format is `.h3t` (a tab-separated spreadsheet). Convert it to the JSON format expected by the generator:

```bash
python3 scripts/h3t_to_json.py input.h3t output.json
# output.json can then be passed to --template
```

If the `.h3t` file contains multiple packs, the output will have a top-level `"packs"` array; single-pack files emit the four keys (`pack`, `map`, `zones`, `connections`) at the top level. `JebusCross.h3t` and `JebusCross.json` in the repository root are a matching example pair.

---

## Quality Checks

### Unit tests

```bash
make units          # build and run tests/Units.cpp
make test_obstacles # build and run tests/TestObstacles.cpp
```

### Pre-commit hooks

```bash
pip install pre-commit
pre-commit install
pre-commit run --all-files   # run all hooks manually
```

---

## Project Structure

```
generator/
├── Generator.cpp               # Entry point
├── global/
│   ├── GridSearch.hpp          # Header-only BFS/Dijkstra/claiming utilities
│   ├── CellularAutomata.*      # Cellular automata helpers
│   ├── FastNoiseLite.hpp       # Noise generation
│   └── Random.*
├── map/
│   ├── Map.*                   # Top-level map model
│   └── placers/                # One placer class per generation stage
│       ├── ZonePlacer.*
│       ├── BorderPlacer.*
│       ├── TownPlacer.*
│       ├── ConnectionPlacer.*
│       ├── ObjectPlacer.*
│       ├── GuardPlacer.*
│       ├── GuardHandler.*
│       └── TerrainPlacer.*
├── mapInfo/                    # Data model: Zone, Tile, Town, Mine, Treasure, …
├── templateInfo/               # Template / Blueprint / Connection parsing
└── luaUtils/                   # Lua script generation helpers
homm3lua/                       # Extended homm3lua C library
homm3tools/                     # Extended homm3tools (h3mlib, h3mtilespritegen)
stats/                          # CRTRAITS, ARTRAITS, obstacleInfo, treasureInfo
scripts/
├── h3t_to_json.py              # Convert .h3t template to JSON
└── convert_crtraits.py
JebusCross.json                 # Example template (also available as JebusCross.h3t)
```

