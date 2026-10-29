#!/usr/bin/env bash
set -e

# PocketDash Run Script

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BIN_DIR="${PROJECT_DIR}/build/bin/PocketDash"
BIN="${BIN_DIR}/PocketDash"

if [[ ! -x "${BIN}" ]]; then
    echo "==> Executable not found. Running build.sh first..."
    "${PROJECT_DIR}/build.sh"
fi

echo "==> Starting PocketDash..."
cd "${BIN_DIR}"
exec "${BIN}" "$@"
