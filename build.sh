#!/usr/bin/env bash
set -e

# PocketDash Linux Desktop Build Script

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${PROJECT_DIR}/build"
BUILD_TYPE="Release"
CLEAN_FIRST=0
RECONFIG=0
JOBS=$(nproc 2>/dev/null || echo 4)

print_usage() {
    echo "Usage: ./build.sh [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -d, --debug       Build in Debug mode (default: Release)"
    echo "  -r, --release     Build in Release mode"
    echo "  -c, --clean       Clean build directory before compiling"
    echo "  --reconfigure     Force CMake reconfiguration"
    echo "  -j, --jobs N      Number of parallel jobs (default: ${JOBS})"
    echo "  -h, --help        Show this help message"
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        -d|--debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        -r|--release)
            BUILD_TYPE="Release"
            shift
            ;;
        -c|--clean)
            CLEAN_FIRST=1
            shift
            ;;
        --reconfigure)
            RECONFIG=1
            shift
            ;;
        -j|--jobs)
            JOBS="$2"
            shift 2
            ;;
        -h|--help)
            print_usage
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            print_usage
            exit 1
            ;;
    esac
done

cd "${PROJECT_DIR}"

if [[ ${CLEAN_FIRST} -eq 1 ]]; then
    echo "==> Cleaning build directory..."
    rm -rf "${BUILD_DIR}"
fi

# Configure if build directory doesn't exist or reconfigure requested
if [[ ! -d "${BUILD_DIR}" || ${RECONFIG} -eq 1 ]]; then
    echo "==> Configuring CMake (${BUILD_TYPE}) with Ninja..."
    cmake -B "${BUILD_DIR}" -S "${PROJECT_DIR}" \
        -G Ninja \
        -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
fi

# Ensure compile_commands.json is symlinked for clangd
if [[ -f "${BUILD_DIR}/compile_commands.json" && ! -L "${PROJECT_DIR}/compile_commands.json" ]]; then
    ln -sf "build/compile_commands.json" "${PROJECT_DIR}/compile_commands.json"
fi

echo "==> Building PocketDash (${BUILD_TYPE}, -j${JOBS})..."
cmake --build "${BUILD_DIR}" --target PocketDash -j"${JOBS}"

echo ""
echo "Build complete! Executable is at:"
echo "  ${BUILD_DIR}/bin/PocketDash/PocketDash"
