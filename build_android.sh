#!/usr/bin/env bash
set -e

# PocketDash Android Build Script

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ANDROID_DIR="${PROJECT_DIR}/proj.android"
ARCH="arm64"
BUILD_TYPE="Debug"
TARGET_AAB=0
INSTALL_DEVICE=0

print_usage() {
    echo "Usage: ./build_android.sh [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -a, --arch ARCH       Target architecture: arm64 (default), armv7, x86_64, or 'arm64;x64'"
    echo "  -r, --release         Build release APK/AAB (default: Debug)"
    echo "  -d, --debug           Build debug APK (default)"
    echo "  --aab                 Build Android App Bundle (.aab) instead of APK"
    echo "  --install             Install APK to connected device/emulator via adb"
    echo "  -h, --help            Show this help message"
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        -a|--arch)
            ARCH="$2"
            shift 2
            ;;
        -r|--release)
            BUILD_TYPE="Release"
            shift
            ;;
        -d|--debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --aab)
            TARGET_AAB=1
            shift
            ;;
        --install)
            INSTALL_DEVICE=1
            shift
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

# Check for Android SDK
if [[ -z "${ANDROID_HOME}" && -z "${ANDROID_SDK_ROOT}" && ! -f "${ANDROID_DIR}/local.properties" ]]; then
    # Check default common locations
    if [[ -d "${HOME}/Android/Sdk" ]]; then
        export ANDROID_HOME="${HOME}/Android/Sdk"
    elif [[ -d "/opt/android-sdk" ]]; then
        export ANDROID_HOME="/opt/android-sdk"
    fi
fi

echo "==> Building for Android (Arch: ${ARCH}, Type: ${BUILD_TYPE})..."

BUILD_ARGS=("-p" "android" "-a" "${ARCH}")

if [[ ${TARGET_AAB} -eq 1 ]]; then
    BUILD_ARGS+=("-aab")
fi

if [[ "${BUILD_TYPE}" == "Release" ]]; then
    BUILD_ARGS+=("-xb" "--config" "Release")
fi

# Run Axmol build command
axmol build "${BUILD_ARGS[@]}"

# Locate resulting APK
APK_PATH="${ANDROID_DIR}/app/build/outputs/apk/${BUILD_TYPE,,}/app-${BUILD_TYPE,,}.apk"

if [[ -f "${APK_PATH}" ]]; then
    echo ""
    echo "Android build succeeded! APK:"
    echo "  ${APK_PATH}"
    
    if [[ ${INSTALL_DEVICE} -eq 1 ]]; then
        if command -v adb >/dev/null 2>&1; then
            echo "==> Installing APK to connected device..."
            adb install -r "${APK_PATH}"
        else
            echo "Warning: adb not found in PATH, skipping install."
        fi
    fi
fi
