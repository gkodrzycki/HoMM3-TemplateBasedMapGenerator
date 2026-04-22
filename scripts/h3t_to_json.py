#!/usr/bin/env python3
"""Convert a HoMM3 .h3t template (tab-separated) into a JSON layout.

The .h3t format has three header rows that describe a hierarchical schema:
    row 1: top-level section names (Pack, Map, Zone, Connections)
    row 2: subsection names within each section
    row 3: leaf field names
    row 4: per-section field counts plus the first data row

After the header, every subsequent row is one Zone + one Connection record.
The Pack and Map sections only have data on the first record row of a pack.
A single .h3t file may contain multiple packs back-to-back; in that case
the output is `{"packs": [ {pack, map, zones, connections}, ... ]}`. For a
single-pack file the four keys are emitted at the top level (backward
compatible).

Usage:
    python3 h3t_to_json.py input.h3t [output.json]
"""

from __future__ import annotations

import csv
import json
import sys
from pathlib import Path
from typing import Any


# Subsections whose leaf fields are checkbox flags. They are emitted as a
# list of selected names rather than an object of booleans. The optional
# `value_fields` set names columns inside such a subsection that are NOT
# checkboxes -- they keep their raw value and are stored alongside the
# flag list under the dedicated key, e.g. Monsters.Strength.
FLAG_LIST_SUBSECTIONS: dict[tuple[str, str], set[str]] = {
    ("Zone", "Town types"): set(),
    ("Zone", "Terrain"): set(),
    ("Zone", "Monsters"): {"Strength"},
    ("Zone", "Allowed Factions"): set(),
}

# Cells whose value is a boolean checkbox (truthy = "x", "X", "1", "true",
# "yes", "+" -- all other non-empty values are kept verbatim).
BOOL_FIELDS = {
    ("Pack", "Options", "Mirror"),
    ("Map", "Tags", "Forbid Hiring Heroes"),
    # In the spreadsheet the following Map fields live in the unnamed
    # subsection ("_"); we register both spellings so they convert no
    # matter how the schema places them.
    ("Map", "Options", "Rock blocks"),
    ("Map", "Options", "Special weeks disabled"),
    ("Map", "Options", "Spell Research"),
    ("Map", "Options", "Anarchy"),
    ("Map", "_", "Rock blocks"),
    ("Map", "_", "Special weeks disabled"),
    ("Map", "_", "Spell Research"),
    ("Map", "_", "Anarchy"),
    ("Zone", "Type", "human start"),
    ("Zone", "Type", "computer start"),
    ("Zone", "Type", "Treasure"),
    ("Zone", "Type", "Junction"),
    ("Zone", "Player towns", "Towns are of same type"),
    ("Zone", "Options", "Force neutral creatures"),
    ("Zone", "Options", "Allow non-coherent road"),
    ("Zone", "Options", "Monsters join only for money"),
    ("Zone", "Options", "Zone repulsion"),
    ("Connections", "Options", "Wide"),
    ("Connections", "Options", "Border Guard"),
    ("Connections", "Options", "Fictive"),
}

# Truthy literals for the checkbox cells above.
_TRUE_LITERALS = {"x", "1", "true", "yes", "y", "+"}

# Fields that should be parsed as integers when present.
INT_FIELDS = {
    ("Pack", "Options", "Max Battle Rounds"),
    ("Map", "_", "Minimum Size"),
    ("Map", "_", "Maximum Size"),
    ("Zone", "_", "Id"),
    ("Zone", "Type", "Base Size"),
    ("Zone", "Restrictions", "Minimum human positions"),
    ("Zone", "Restrictions", "Maximum human positions"),
    ("Zone", "Restrictions", "Minimum total positions"),
    ("Zone", "Restrictions", "Maximum total positions"),
    ("Zone", "Player towns", "Ownership"),  # 1-based player index, 0 = neutral
    ("Zone", "Type", "Ownership"),
    ("Zone", "Player towns", "Minimum towns"),
    ("Zone", "Player towns", "Minimum castles"),
    ("Zone", "Player towns", "Town Density"),
    ("Zone", "Player towns", "Castle Density"),
    ("Zone", "Neutral towns", "Minimum towns"),
    ("Zone", "Neutral towns", "Minimum castles"),
    ("Zone", "Neutral towns", "Town Density"),
    ("Zone", "Neutral towns", "Castle Density"),
    ("Zone", "Minimum mines", "Wood"),
    ("Zone", "Minimum mines", "Mercury"),
    ("Zone", "Minimum mines", "Ore"),
    ("Zone", "Minimum mines", "Sulfur"),
    ("Zone", "Minimum mines", "Crystal"),
    ("Zone", "Minimum mines", "Gems"),
    ("Zone", "Minimum mines", "Gold"),
    ("Zone", "Mine Density", "Wood"),
    ("Zone", "Mine Density", "Mercury"),
    ("Zone", "Mine Density", "Ore"),
    ("Zone", "Mine Density", "Sulfur"),
    ("Zone", "Mine Density", "Crystal"),
    ("Zone", "Mine Density", "Gems"),
    ("Zone", "Mine Density", "Gold"),
    ("Zone", "Treasure", "Low"),
    ("Zone", "Treasure", "High"),
    ("Zone", "Treasure", "Density"),
    ("Zone", "Options", "Minimum objects"),
    ("Zone", "Options", "Monsters disposition (standard)"),
    ("Zone", "Options", "Monsters joining percentage"),
    ("Zone", "Options", "Minimum airship shipyards"),
    ("Zone", "Options", "Airship shipyard Density"),
    ("Zone", "Options", "Max block value"),
    ("Connections", "Zones", "Zone 1"),
    ("Connections", "Zones", "Zone 2"),
    ("Connections", "Options", "Value"),
    ("Connections", "Options", "Portal repulsion"),
    ("Connections", "Restrictions", "Minimum human positions"),
    ("Connections", "Restrictions", "Maximum human positions"),
    ("Connections", "Restrictions", "Minimum total positions"),
    ("Connections", "Restrictions", "Maximum total positions"),
}

# Subsection holding bookkeeping counters in row 4 (cells 1..7). Stripped.
_FIELD_COUNT_SUBSECTION = "Field count"

# Encodings tried in order when reading a .h3t file. HotA editor exports
# are usually UTF-8 but Russian-locale exports may be cp1251 or cp1252.
_ENCODINGS = ("utf-8-sig", "utf-8", "cp1251", "cp1252", "latin-1")


def _read_rows(path: Path) -> list[list[str]]:
    last_err: Exception | None = None
    for enc in _ENCODINGS:
        try:
            with path.open("r", encoding=enc, newline="") as f:
                return list(csv.reader(f, delimiter="\t", quoting=csv.QUOTE_NONE))
        except UnicodeDecodeError as exc:
            last_err = exc
            continue
    raise RuntimeError(f"Could not decode {path}: {last_err}")


def parse_headers(rows: list[list[str]]) -> list[tuple[str, str, str]]:
    """Return a (section, subsection, field) triple for every column."""
    section_row, subsection_row, field_row = rows[0], rows[1], rows[2]
    width = max(len(section_row), len(subsection_row), len(field_row))

    def pad(r: list[str]) -> list[str]:
        return r + [""] * (width - len(r))

    section_row = pad(section_row)
    subsection_row = pad(subsection_row)
    field_row = pad(field_row)

    triples: list[tuple[str, str, str]] = []
    cur_section = ""
    cur_subsection = ""
    for i in range(width):
        if section_row[i].strip():
            cur_section = section_row[i].strip()
            cur_subsection = ""
        if subsection_row[i].strip():
            cur_subsection = subsection_row[i].strip()
        triples.append((cur_section, cur_subsection, field_row[i].strip()))
    return triples


def coerce(value: str, key: tuple[str, str, str]) -> Any:
    v = value.strip()
    if v == "":
        return None
    if key in BOOL_FIELDS:
        return v.lower() in _TRUE_LITERALS
    if key in INT_FIELDS:
        try:
            return int(v)
        except ValueError:
            try:
                return float(v)
            except ValueError:
                return v
    return v


def group_subsection_columns(
    triples: list[tuple[str, str, str]], target_section: str
) -> dict[str, list[list[int]]]:
    """For each subsection, return groups of column indices.

    A new group is started whenever a leaf field name repeats within the
    same subsection (e.g. Treasure has 3 Low/High/Density tiers).
    """
    groups: dict[str, list[list[int]]] = {}
    cur_fields: dict[str, set[str]] = {}
    for col, (section, subsection, field) in enumerate(triples):
        if section != target_section:
            continue
        if subsection == _FIELD_COUNT_SUBSECTION:
            continue  # Strip bookkeeping counters from the schema entirely.
        sub_key = subsection or "_"
        bucket = groups.setdefault(sub_key, [[]])
        seen = cur_fields.setdefault(sub_key, set())
        if field in seen:
            bucket.append([])
            seen = set()
            cur_fields[sub_key] = seen
        bucket[-1].append(col)
        seen.add(field)
    return groups


def collect_record(
    row: list[str],
    triples: list[tuple[str, str, str]],
    target_section: str,
    groups: dict[str, list[list[int]]],
) -> dict[str, Any]:
    """Build a nested dict for one section of one row.

    Subsections that contain repeated fields are emitted as a list of
    dicts (one per tier/group). Fields with no subsection header are
    flattened into the parent record.
    """
    out: dict[str, Any] = {}
    for sub_key, tier_columns in groups.items():
        section_key = (target_section, sub_key)

        if section_key in FLAG_LIST_SUBSECTIONS:
            value_fields = FLAG_LIST_SUBSECTIONS[section_key]
            flags: list[str] = []
            scalars: dict[str, Any] = {}
            for col in (c for tier in tier_columns for c in tier):
                if col >= len(row):
                    continue
                raw = row[col].strip()
                if raw == "":
                    continue
                field_name = triples[col][2]
                if field_name in value_fields:
                    key = (target_section, sub_key, field_name)
                    coerced = coerce(raw, key)
                    if coerced is not None:
                        scalars[field_name] = coerced
                elif raw.lower() in _TRUE_LITERALS:
                    flags.append(field_name)
            if flags and scalars:
                out[sub_key] = {**scalars, "list": flags}
            elif flags:
                out[sub_key] = flags
            elif scalars:
                out[sub_key] = scalars
            continue

        tier_results: list[dict[str, Any]] = []
        for tier in tier_columns:
            tier_dict: dict[str, Any] = {}
            for col in tier:
                if col >= len(row):
                    continue
                raw = row[col]
                if raw.strip() == "":
                    continue
                key = (target_section, sub_key, triples[col][2])
                value = coerce(raw, key)
                if value is None:
                    continue
                tier_dict[triples[col][2]] = value
            tier_results.append(tier_dict)

        non_empty = [t for t in tier_results if t]
        if not non_empty:
            continue
        if len(tier_results) == 1 or len(non_empty) == 1:
            value: Any = non_empty[0]
        else:
            value = non_empty

        if sub_key == "_" and isinstance(value, dict):
            out.update(value)
        else:
            out[sub_key] = value
    return out


def _row_has_section_data(
    row: list[str],
    triples: list[tuple[str, str, str]],
    target_section: str,
) -> bool:
    """True if any non-bookkeeping cell in the given section is filled."""
    for col, (section, subsection, _field) in enumerate(triples):
        if section != target_section or subsection == _FIELD_COUNT_SUBSECTION:
            continue
        if col < len(row) and row[col].strip():
            return True
    return False


def _build_pack(
    pack_rows: list[list[str]],
    triples: list[tuple[str, str, str]],
    pack_groups: dict[str, list[list[int]]],
    map_groups: dict[str, list[list[int]]],
    zone_groups: dict[str, list[list[int]]],
    conn_groups: dict[str, list[list[int]]],
) -> dict[str, Any]:
    pack = collect_record(pack_rows[0], triples, "Pack", pack_groups)
    map_info = collect_record(pack_rows[0], triples, "Map", map_groups)

    zones: list[dict[str, Any]] = []
    connections: list[dict[str, Any]] = []
    for row in pack_rows:
        z = collect_record(row, triples, "Zone", zone_groups)
        if z:
            zones.append(z)
        c = collect_record(row, triples, "Connections", conn_groups)
        if c:
            connections.append(c)

    return {
        "pack": pack,
        "map": map_info,
        "zones": zones,
        "connections": connections,
    }


def convert(h3t_path: Path) -> dict[str, Any]:
    rows = _read_rows(h3t_path)
    if len(rows) < 4:
        raise ValueError(f"{h3t_path}: file is too short to be a .h3t template")

    triples = parse_headers(rows)
    pack_groups = group_subsection_columns(triples, "Pack")
    map_groups = group_subsection_columns(triples, "Map")
    zone_groups = group_subsection_columns(triples, "Zone")
    conn_groups = group_subsection_columns(triples, "Connections")

    # Row 4 onward is data. Drop fully-empty trailing rows.
    data_rows = [r for r in rows[3:] if any(c.strip() for c in r)]
    if not data_rows:
        return {"pack": {}, "map": {}, "zones": [], "connections": []}

    # Multi-pack support: a row beyond the first that has Pack-section
    # data starts a new pack. We rely on data presence rather than the
    # rarely-populated "Pack new" flag column.
    pack_chunks: list[list[list[str]]] = [[data_rows[0]]]
    for row in data_rows[1:]:
        if _row_has_section_data(row, triples, "Pack"):
            pack_chunks.append([row])
        else:
            pack_chunks[-1].append(row)

    packs = [
        _build_pack(chunk, triples, pack_groups, map_groups, zone_groups, conn_groups)
        for chunk in pack_chunks
    ]

    if len(packs) == 1:
        return packs[0]
    return {"packs": packs}


def main(argv: list[str]) -> int:
    if len(argv) < 2:
        print(__doc__)
        return 2
    src = Path(argv[1])
    dst = Path(argv[2]) if len(argv) > 2 else src.with_suffix(".json")
    result = convert(src)
    dst.write_text(json.dumps(result, indent=2, ensure_ascii=False) + "\n",
                   encoding="utf-8")
    print(f"Wrote {dst}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
