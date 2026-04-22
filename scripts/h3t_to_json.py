#!/usr/bin/env python3
"""Convert a HoMM3 .h3t template (tab-separated) into a JSON layout.

The .h3t format has three header rows that describe a hierarchical schema:
    row 1: top-level section names (Pack, Map, Zone, Connections)
    row 2: subsection names within each section
    row 3: leaf field names
    row 4: per-section field counts plus the first data row

After the header, every subsequent row is one Zone + one Connection record.
The Pack and Map sections only have data on the first record row.

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
# list of selected names rather than an object of booleans.
FLAG_LIST_SUBSECTIONS = {
    ("Zone", "Town types"),
    ("Zone", "Terrain"),
    ("Zone", "Monsters"),
    ("Zone", "Allowed Factions"),
}

# Cells whose value is a boolean checkbox ("x" means true).
BOOL_FIELDS = {
    ("Pack", "Options", "Mirror"),
    ("Map", "Tags", "Forbid Hiring Heroes"),
    ("Zone", "Type", "human start"),
    ("Zone", "Type", "computer start"),
    ("Zone", "Type", "Treasure"),
    ("Zone", "Type", "Junction"),
    ("Zone", "Player towns", "Towns are of same type"),
    ("Zone", "Options", "Force neutral creatures"),
    ("Zone", "Options", "Allow non-coherent road"),
    ("Zone", "Options", "Monsters join only for money"),
    ("Connections", "Options", "Wide"),
    ("Connections", "Options", "Border Guard"),
    ("Connections", "Options", "Fictive"),
}

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
    ("Zone", "Options", "Zone repulsion"),
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
        return v.lower() == "x"
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
    """Build a nested dict {subsection: {field: value}} for one section.

    Subsections that contain repeated fields are emitted as a list of
    dicts (one per tier/group).
    """
    out: dict[str, Any] = {}
    for sub_key, tier_columns in groups.items():
        section_key = (target_section, sub_key)

        if section_key in FLAG_LIST_SUBSECTIONS:
            flags: list[str] = []
            for col in (c for tier in tier_columns for c in tier):
                if col >= len(row):
                    continue
                if row[col].strip().lower() == "x":
                    flags.append(triples[col][2])
            if flags:
                out[sub_key] = flags
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
        if len(tier_results) == 1:
            value = non_empty[0]
        else:
            value = non_empty if len(non_empty) > 1 else non_empty[0]

        # Fields that have no subsection header are stored under "_" by
        # the parser. Flatten them straight into the parent record so the
        # JSON reads more naturally (e.g. a zone's `Id` becomes a top
        # level key instead of `"_": { "Id": ... }`).
        if sub_key == "_" and isinstance(value, dict):
            for k, v in value.items():
                out[k] = v
        else:
            out[sub_key] = value
    return out


def convert(h3t_path: Path) -> dict[str, Any]:
    with h3t_path.open("r", encoding="utf-8", newline="") as f:
        rows = list(csv.reader(f, delimiter="\t", quoting=csv.QUOTE_NONE))

    triples = parse_headers(rows)
    pack_groups = group_subsection_columns(triples, "Pack")
    map_groups = group_subsection_columns(triples, "Map")
    zone_groups = group_subsection_columns(triples, "Zone")
    conn_groups = group_subsection_columns(triples, "Connections")

    data_rows = rows[3:]  # row 4 onwards (includes the "Field count" row)
    if data_rows and data_rows[0] and data_rows[0][0].lower() == "field count":
        first_data_row = data_rows[0]
        remaining = data_rows[1:]
    else:
        first_data_row = data_rows[0]
        remaining = data_rows[1:]

    pack = collect_record(first_data_row, triples, "Pack", pack_groups)
    map_info = collect_record(first_data_row, triples, "Map", map_groups)

    zones: list[dict[str, Any]] = []
    connections: list[dict[str, Any]] = []

    for row in [first_data_row, *remaining]:
        z = collect_record(row, triples, "Zone", zone_groups)
        if z:
            zones.append(z)
        c = collect_record(row, triples, "Connections", conn_groups)
        if c:
            connections.append(c)

    # Drop bookkeeping fields from Pack ("Field count" subsection counters).
    pack.pop("Field count", None)

    return {
        "pack": pack,
        "map": map_info,
        "zones": zones,
        "connections": connections,
    }


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
