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
from pathlib import Path
from typing import Any

def _normalize_header(name: str) -> str:
    """Normalize a column header to a stable key.

    Examples:
      "Fight Value" -> "fight_value"
      "AI Value" -> "ai_value"
    """

    s = name.strip().lower()
    s = "_".join(s.split())
    out: list[str] = []
    prev_us = False
    for ch in s:
        ok = ("a" <= ch <= "z") or ("0" <= ch <= "9")
        if ok:
            out.append(ch)
            prev_us = False
        else:
            if not prev_us:
                out.append("_")
                prev_us = True
    normalized = "".join(out).strip("_")
    while "__" in normalized:
        normalized = normalized.replace("__", "_")
    return normalized


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


def parse_crtraits_selected(path: Path, fields: list[str]) -> tuple[list[str], list[list[str]]]:
    """Parse CRTRAITS.TXT and return only requested columns.

    CRTRAITS.TXT has two header rows. We use the *second* row (actual column names).
    The file is TSV but may contain quoted fields with embedded newlines.
    """

    with path.open("r", encoding="utf-8", errors="replace", newline="") as f:
        reader = csv.reader(f, delimiter="\t", quotechar='"')

        try:
            _ = next(reader)
            column_header = next(reader)
        except StopIteration as e:
            raise ValueError(f"{path} does not contain the expected 2 header rows") from e

        normalized_to_index: dict[str, int] = {}
        for i, name in enumerate(column_header):
            key = _normalize_header(name)
            if key and key not in normalized_to_index:
                normalized_to_index[key] = i

        missing = [k for k in fields if k not in normalized_to_index]
        if missing:
            available = ", ".join(sorted(normalized_to_index.keys()))
            raise ValueError(
                f"Requested fields not found: {missing}. Available fields: {available}"
            )

        indices = [normalized_to_index[k] for k in fields]
        rows: list[list[str]] = []
        for row in reader:
            if not row or all(c.strip() == "" for c in row):
                continue
            rows.append([row[i] if i < len(row) else "" for i in indices])

        return fields, rows


def write_csv(out_path: Path, keys: list[str], rows: list[list[str]]) -> None:
    out_path.parent.mkdir(parents=True, exist_ok=True)
    with out_path.open("w", encoding="utf-8", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(keys)
        for row in rows:
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

    try:
        keys, rows = parse_crtraits_selected(in_path, args.fFields or [])
    except ValueError as e:
        ap.error(str(e))

    if args.out_csv:
        write_csv(Path(args.out_csv), keys, rows)
    if args.out_json:
        write_json(Path(args.out_json), keys, rows)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
