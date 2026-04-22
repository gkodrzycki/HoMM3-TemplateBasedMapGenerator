# `.h3t` Template Format — Reference & JSON Mapping

`.h3t` is the **HotA** (Horn of the Abyss) random map template format used by
the HotA Map Editor. It is a **tab-separated values** file with a 3-row
hierarchical header (section / subsection / field) followed by data rows.

The `scripts/h3t_to_json.py` converter in this repo emits a JSON file that
mirrors the schema 1:1, e.g. [JebusCross.h3t](../JebusCross.h3t) →
[JebusCross.json](../JebusCross.json).

> ⚠️ The `.h3t` format is a community/HotA-internal format with no formal
> public spec. Field meanings below are reconstructed from the column
> headers and from observed values in known templates. A few low-level
> encodings (notably the `Objects` strings) are only partially documented;
> see the dedicated section for what is known.

---

## File layout

```
Row 1   Section      : Pack | Map | Zone | Connections
Row 2   Subsection   : Options | Type | Restrictions | Player towns | …
Row 3   Field        : the actual leaf field name
Row 4   Field count  : per-section counters (cols 1-7) + first data row
Row 5+  Data rows    : one row per Zone and one Connection per row
```

The first data row carries `Pack` + `Map` data (which only appear once)
*and* the first Zone / first Connection. Subsequent rows only hold a Zone
and a Connection each.

A boolean checkbox in `.h3t` is the literal character `x` in a cell. Empty
cells mean "unset / use default".

---

## Top-level sections

### `pack` — template package metadata
| Field | Meaning |
| --- | --- |
| `Name` | Display name of the template pack. |
| `Description` | Long form description shown in the launcher / map selector. |
| `Town selection` | Town pool / selection rule (rarely used). |
| `Heroes` | Hero pool override. |
| `Mirror` | If `true`, all player zones are mirrored layouts. |
| `Tags` | Free-form tag string. |
| `Max Battle Rounds` | Combat round limit (`100` = HotA default cap). |
| `Forbid Hiring Heroes` | Disables hero recruiting in taverns. |

### `map` — per-map options
| Field | Meaning |
| --- | --- |
| `Name` | Map name (often duplicates `pack.Name`). |
| `Minimum Size` / `Maximum Size` | Map size codes. `9 = S`, `12 = M`, `15 = L`, `16 = XL`, etc. (HotA also uses 18 = G, 21 = H). |
| `Artifacts` / `Combo Arts` / `Spells` / `Secondary skills` | Allowed-pool selection strings. |
| `Objects` | Forced map-wide objects. See [Objects strings](#objects-strings). |
| `Rock blocks` | Whether large impassable rock walls are used. |
| `Zone sparseness` | How loosely zones may be packed. |
| `Special weeks disabled` | Disables "Week of …" events. |
| `Spell Research` | HotA spell research mechanic toggle. |
| `Anarchy` | HotA "no faction limit" hero mode. |

### `zones[]` — one entry per zone
| Subsection | Fields | Meaning |
| --- | --- | --- |
| (top-level) | `Id` | Numeric zone id, referenced by `connections`. |
| `Type` | `human start`, `computer start`, `Treasure`, `Junction`, `Base Size` | Zone role + base size in tiles (e.g. 30, 40). Exactly one of the role flags is normally set. `Junction` zones are the connector hubs. |
| `Restrictions` | `Minimum/Maximum human positions`, `Minimum/Maximum total positions` | Player-count bounds for which this zone instance is generated. |
| `Player towns` | `Ownership`, `Minimum towns`, `Minimum castles`, `Town Density`, `Castle Density`, `Towns are of same type` | Town generation for the **owning** player. `Ownership` is a 1-based player index; `0` means neutral. |
| `Neutral towns` | `Minimum towns`, `Minimum castles`, `Town Density`, `Castle Density` | Town generation for **neutral** towns added to this zone. |
| `Town types` | List of factions | Allowed town factions for this zone. Encoded as an array of allowed faction names. |
| `Minimum mines` | `Wood`, `Mercury`, `Ore`, `Sulfur`, `Crystal`, `Gems`, `Gold` | Guaranteed mine counts of each resource type. |
| `Mine Density` | same fields | Density weights for additional mines. |
| `Terrain` | `Match to town`, `Dirt`, `Sand`, `Grass`, `Snow`, `Swamp`, `Rough`, `Cave`, `Lava`, `Highlands`, `Wasteland` | Allowed terrains. `Match to town` means "use the owning faction's native terrain". |
| `Monsters` | `Strength`, `Match to town`, `Neutral`, *faction names…* | Guard creature pool + difficulty (`weak`/`normal`/`strong`). `Match to town` lets the zone use the owner faction's native creatures. |
| `Treasure` | tiers of `{Low, High, Density}` | Each tier is one (gold-value range, density) pair. Three tiers are typical: low / mid / high value piles. Emitted as a JSON array. |
| `Options` | `Placement`, `Objects`, `Minimum objects`, `Image settings`, `Force neutral creatures`, `Allow non-coherent road`, `Zone repulsion`, `Town Hint`, `Monsters disposition (standard)`, `Monsters disposition (custom)`, `Monsters joining percentage`, `Monsters join only for money`, `Minimum airship shipyards`, `Airship shipyard Density`, `Terrain Hint`, `Faction Hint`, `Max block value` | Misc per-zone toggles. `Image settings` is `x y z size` for the editor's preview canvas. `Monsters disposition (standard)` is 1=savage, 2=hostile, 3=aggressive, 4=friendly (HotA disposition codes). |
| `Allowed Factions` | List of faction names | Restricts the *town/hero* faction pool when stricter than `Town types`. |

### `connections[]` — links between zones
| Subsection | Fields | Meaning |
| --- | --- | --- |
| `Zones` | `Zone 1`, `Zone 2` | Endpoint zone ids. |
| `Options` | `Value`, `Wide`, `Border Guard`, `Road`, `Type`, `Fictive`, `Portal repulsion` | Connection cost/value, whether it's a wide passage, has a border guard, has a road; `Type` is the connection kind (regular / monolith / subterranean gate / etc.); `Fictive` connections only express adjacency without a passage. |
| `Restrictions` | `Minimum/Maximum human positions`, `Minimum/Maximum total positions` | Same as zone restrictions: the connection only exists for these player-count configurations. |

---

## Why some fields used to live under `"_"`

The h3t spreadsheet has a few leaf fields whose **subsection header is
empty** — most notably `Map.Name/Minimum Size/Maximum Size` and the
zone's `Id`. The parser originally stored those under a synthetic
subsection named `"_"`. They are now flattened straight into the parent
record so the JSON reads naturally:

```json
"zones": [ { "Id": 1, "Type": { … } } ]
```

---

## `Objects` strings

You will see entries like:

```text
+95 0 d d d 3 d +144 4 d d d 3 d +144 5 d d d 3 d
```

This is HotA's **forced-object spec**, used both at map level
(`map.Objects`) and per zone (`zones[].Options.Objects`). It is a
whitespace-separated list of object descriptors; each descriptor begins
with `+` (meaning "place this object") and is followed by **7 fields**:

```
+<objectId> <subtype> <p1> <p2> <p3> <p4> <p5>
```

| Token | Meaning |
| --- | --- |
| `+<id>` | The HoMM3 object class id. `95` = Subterranean Gate, `144` = Monolith Two-Way (these ids match the H3M `OA_*` enum used by the engine). The leading `+` means "force-place"; some templates also use `-` to forbid an object. |
| `<subtype>` | Object subtype / appearance variant. For monoliths this is the colour group (so two `+144` entries with subtypes `4` and `5` create two independent monolith pairs). For most other objects `0`. |
| `<p1>…<p5>` | Five generation parameters. The literal letter `d` means "default" — let the generator pick. Where a number appears it overrides one parameter. The five slots correspond (in order) to: target zone restriction, value class, density, group/owner, and a flags byte. The exact semantics differ per object id, which is why most templates leave them as `d`. |

Worked example for `+95 0 d d d 3 d`:

* `+95` — place a Subterranean Gate.
* `0` — default subtype.
* `d d d` — leave the first three params at default.
* `3` — set the 4th param (group) to `3`, meaning the gate belongs to
  group #3 (gates of the same group connect to each other).
* `d` — default flags.

So the full JebusCross map-level string

```text
+95 0 d d d 3 d  +144 4 d d d 3 d  +144 5 d d d 3 d
```

means: *"place one Subterranean Gate (group 3), one Monolith Two-Way pair
of colour 4 (group 3), and one Monolith Two-Way pair of colour 5 (group
3) somewhere on the map."*

> The 5-parameter portion is the least-documented part of the format. The
> mapping above (zone / value / density / group / flags) is the working
> interpretation used by HotA template authors and is consistent with
> the values you see in well-known templates, but the engine treats the
> tuple per-object-id, so unusual ids may interpret slots differently.

---

## Faction & terrain code reference

`Town types`, `Monsters` and `Allowed Factions` use full faction names:
`Castle, Rampart, Tower, Inferno, Necropolis, Dungeon, Stronghold,
Fortress, Conflux, Cove, Factory, Bulwark`. (`Cove`, `Factory`, `Bulwark`
are HotA-only.)

Terrain names match the in-game tile types: `Dirt, Sand, Grass, Snow,
Swamp, Rough, Cave (Subterranean), Lava, Highlands, Wasteland`.
`Match to town` is a meta-value that resolves to the owner's native
terrain at generation time.

---

## Converter usage

```sh
python3 scripts/h3t_to_json.py JebusCross.h3t JebusCross.json
```

The output JSON has four top-level keys — `pack`, `map`, `zones`,
`connections` — that mirror the four sections of the `.h3t` header.
Subsections become nested objects; checkbox-list subsections (`Town
types`, `Terrain`, `Monsters`, `Allowed Factions`) become arrays of
selected names; the `Treasure` block becomes an array of tier objects.
