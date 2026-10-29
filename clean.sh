#!/usr/bin/env bash
set -e

# PocketDash Clean Script

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "==> Cleaning desktop build directory..."
rm -rf "${PROJECT_DIR}/build"

echo "==> Cleaning Android build artifacts..."
rm -rf "${PROJECT_DIR}/proj.android/app/build"
rm -rf "${PROJECT_DIR}/proj.android/.cxx"
rm -rf "${PROJECT_DIR}/proj.android/.gradle"

echo "Clean done!"
