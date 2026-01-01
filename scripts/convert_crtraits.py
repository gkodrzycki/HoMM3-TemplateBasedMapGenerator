"""
Convert HoMM3 CRTRAITS.TXT (tab-separated, quoted, multiline) to CSV and/or JSON.

Why this exists:
- We want to have some idea about creature powers when generating maps, to balance gameplay.

Example usage:
  python3 scripts/convert_crtraits.py \
    --input CRTRAITS.TXT \
    --out-csv crtraits.csv \
    --out-json crtraits.json

Notes:
- CSV output keeps all values as strings.
- JSON output tries to convert integer-looking fields to ints.
- I got CRTRAITS.TXT by running vcmiclient, using command 'get txt' in console and copying CRTRAITS.TXT from provided directory.
"""

import argparse
import csv
import json
import re
from pathlib import Path
from typing import Any

_NON_ALNUM_RE = re.compile(r"[^0-9a-zA-Z]+")


def _slugify(name: str) -> str:
    name = name.strip()
    name = _NON_ALNUM_RE.sub("_", name)
    name = name.strip("_")
    name = re.sub(r"_+", "_", name)
    return name.lower() or "col"


def _propagate_groups(group_row: list[str], ncols: int) -> list[str]:
    groups: list[str] = [""] * ncols
    current = ""
    for i in range(ncols):
        cell = group_row[i].strip() if i < len(group_row) else ""
        if cell:
            current = cell
        groups[i] = current
    return groups


def _make_unique_keys(groups: list[str], header: list[str]) -> list[str]:
    """Return stable, unique column keys based on group + header names."""

    keys: list[str] = []
    seen: dict[str, int] = {}

    for i, raw_name in enumerate(header):
        base = raw_name.strip()
        group = groups[i].strip() if i < len(groups) else ""

        if not base:
            base = f"Column {i + 1}"

        # Prefer group prefix when name is very generic or duplicated.
        base_key = _slugify(base)
        if group:
            group_key = _slugify(group)
            if base_key in {"low", "high"}:
                candidate = f"{group_key}_{base_key}"
            else:
                candidate = base_key
        else:
            candidate = base_key

        # Ensure uniqueness.
        count = seen.get(candidate, 0)
        if count == 0:
            unique = candidate
        else:
            unique = f"{candidate}_{count + 1}"
        seen[candidate] = count + 1
        keys.append(unique)

    return keys


def _maybe_int(value: str) -> int | str:
    v = value.strip()
    if v == "":
        return ""
    # Allow leading '-' but disallow things like '+1 morale.'
    if v.startswith("-"):
        digits = v[1:]
        if digits.isdigit():
            return int(v)
        return value
    if v.isdigit():
        return int(v)
    return value


def parse_crtraits(path: Path) -> tuple[list[str], list[list[str]]]:
    """Parse CRTRAITS.TXT into (keys, rows) where rows are raw string lists."""

    with path.open("r", encoding="utf-8", errors="replace", newline="") as f:
        reader = csv.reader(f, delimiter="\t", quotechar='"')

        try:
            group_header = next(reader)
            column_header = next(reader)
        except StopIteration as e:
            raise ValueError(
                f"{path} does not contain the expected 2 header rows"
            ) from e

        ncols = max(len(group_header), len(column_header))
        groups = _propagate_groups(group_header, ncols)
        keys = _make_unique_keys(
            groups, column_header + [""] * (ncols - len(column_header))
        )

        rows: list[list[str]] = []
        for row in reader:
            if not row or all(c.strip() == "" for c in row):
                continue
            if len(row) < ncols:
                row = row + [""] * (ncols - len(row))
            elif len(row) > ncols:
                # Keep extra columns rather than dropping data.
                extra = len(row) - ncols
                for j in range(extra):
                    keys.append(f"extra_{j + 1}")
                ncols = len(row)
            rows.append(row)

        return keys, rows


def select_fields(
    keys: list[str], rows: list[list[str]], fields: list[str]
) -> tuple[list[str], list[list[str]]]:
    """Filter (keys, rows) down to the requested fields, preserving order."""

    if not fields:
        return keys, rows

    key_to_index = {k: i for i, k in enumerate(keys)}
    missing = [f for f in fields if f not in key_to_index]
    if missing:
        available = ", ".join(keys)
        raise ValueError(
            f"Requested fields not found: {missing}. Available keys: {available}"
        )

    indices = [key_to_index[f] for f in fields]
    out_keys = [keys[i] for i in indices]
    out_rows = [[row[i] if i < len(row) else "" for i in indices] for row in rows]
    return out_keys, out_rows


def write_csv(out_path: Path, keys: list[str], rows: list[list[str]]) -> None:
    out_path.parent.mkdir(parents=True, exist_ok=True)
    with out_path.open("w", encoding="utf-8", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(keys)
        for row in rows:
            # Always keep CSV as strings.
            writer.writerow(["" if v is None else str(v) for v in row])


def write_json(out_path: Path, keys: list[str], rows: list[list[str]]) -> None:
    out_path.parent.mkdir(parents=True, exist_ok=True)

    data: list[dict[str, Any]] = []
    for row in rows:
        obj: dict[str, Any] = {}
        for k, v in zip(keys, row, strict=False):
            obj[k] = _maybe_int(v)
        data.append(obj)

    with out_path.open("w", encoding="utf-8") as f:
        json.dump(data, f, ensure_ascii=False, indent=2)
        f.write("\n")


def main() -> int:
    ap = argparse.ArgumentParser(description="Convert CRTRAITS.TXT to CSV and/or JSON")
    ap.add_argument("--input", default="CRTRAITS.TXT", help="Path to CRTRAITS.TXT")
    ap.add_argument("--out-csv", default="CRTRAITS.csv", help="Write CSV to this path")
    ap.add_argument(
        "--out-json", default="CRTRAITS.json", help="Write JSON to this path"
    )
    ap.add_argument(
        "--fields",
        nargs="+",
        default=["singular", "plural", "fight_value", "ai_value"],
        help="Only output these keys (e.g. --fields fight_value ai_value)",
    )

    args = ap.parse_args()

    in_path = Path(args.input)
    if not in_path.exists():
        ap.error(f"Input file not found: {in_path}")

    if not args.out_csv and not args.out_json:
        ap.error("At least one of --out-csv or --out-json must be provided")

    keys, rows = parse_crtraits(in_path)
    if args.fields:
        keys, rows = select_fields(keys, rows, args.fields)

    if args.out_csv:
        write_csv(Path(args.out_csv), keys, rows)
    if args.out_json:
        write_json(Path(args.out_json), keys, rows)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
