#!/usr/bin/env bash
set -e

echo "[pre-commit] Sprawdzam formatowanie..."
make format-check

echo "[pre-commit] OK :)"