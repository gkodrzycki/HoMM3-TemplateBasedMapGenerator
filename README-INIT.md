## Abstract
HoMM3-MapGen is a **procedural Heroes of Might and Magic III map generator** that turns a high-level **JSON layout template** (regions, zones, and their connections) into a playable **.H3M** map. The generator is implemented in **C++**, uses a **seedable RNG** for reproducibility, and emits a **Lua script** that builds the map via **homm3lua**, producing an output compatible with **VCMI**.

## Background / Motivation
This project is aiming to refactor and improve work done as [Engineering Thesis](https://github.com/Sko0owi/HOMM3-mapgen/tree/main).

### Important References / Bibliography
- **Designing a Template-Based Map Generator for Heroes of Might & Magic III** [Thesis](https://jakubkowalski.tech/Supervising/Skowronek2025DesigningTemplateBased.pdf)
- **VCMI** (open-source HoMM3 engine; target runtime for generated maps)
- **homm3tools / homm3lua** (Lua API and tooling used to write H3M files)
- **Lua** (runtime for the generated build script)
- **nlohmann/json** (JSON parsing library used for `layout.json`)

## Project Goal
Primary goal: generate a valid HoMM3/VCMI map from a template describing:

- Map metadata (name, description, difficulty, size)
- Regions and zones (including terrain, faction/ownership for starting zones)
- Connectivity between zones (roads / links)

Expected outcomes:

- A repeatable pipeline: `layout.json` → C++ generator → `generated_script.lua` → `.h3m`
- A template system that can be extended to support richer zone placement rules, obstacles, and content density constraints

## Methodology (Ideas and Algorithms)
This section reflects the **current implementation** plus the intended direction for improvements.

### Current implementation
1. **Template parsing (JSON → in-memory layout)**
	- `layout.json` is deserialized into `LayoutInfo`, containing a list of `RegionInfo` and `ConnectionInfo`.

2. **Zone seeding and region creation**
	- Regions and zones are created from the template.
	- Zone centers are currently assigned with simple fixed positions (temporary approach; marked TODO in code).

3. **Terrain claiming via multi-source flood fill (BFS)**
	- Given zone centers, tiles are “claimed” by the nearest zone using a queue-based expansion over 4-neighborhood (a multi-source BFS flood fill).
	- Each tile receives a `zoneID` and terrain type based on its owning zone.

4. **Object placement (rule-based)**
	- **Towns**: starting zones place a town object at the zone center.
	- **Borders**: border tiles are detected by scanning 8-neighborhood terrain changes, expanded outward (bounded BFS), and obstacles are placed along borders while avoiding road overlap.

5. **Road placement via grid BFS pathing**
	- For each declared connection, a shortest path on the tile grid is created using BFS over 4-neighborhood.
	- Town footprints are treated as blocked cells (basic collision avoidance).
	- Road objects are then instantiated along the found path.

6. **Export (C++ → Lua → H3M)**
	- A Lua script is emitted which uses `homm3lua` to create an H3M map with the requested size and metadata.
	- The Lua script is executed from the generator, writing the final `.h3m` to a default VCMI maps directory.

### Planned improvements
- **Smarter zone center placement**
- **Cost-aware pathfinding**
- **Constraint-driven content placement**
- **Richer region topology controls**
