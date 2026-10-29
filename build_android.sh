#!/usr/bin/env bash
set -e

# PocketDash Android Build Script

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ANDROID_DIR="${PROJECT_DIR}/proj.android"
ARCH="arm64"
BUILD_TYPE="Debug"
TARGET_AAB=0
INSTALL_DEVICE=0

# Export default Android SDK & NDK paths if not set
export ANDROID_HOME="${ANDROID_HOME:-$HOME/Android/Sdk}"
export ANDROID_SDK_ROOT="${ANDROID_SDK_ROOT:-$ANDROID_HOME}"
export ANDROID_NDK_HOME="${ANDROID_NDK_HOME:-$ANDROID_HOME/ndk/27.3.13750724}"
export ANDROID_NDK_ROOT="${ANDROID_NDK_ROOT:-$ANDROID_NDK_HOME}"
export PATH="/usr/bin:${ANDROID_HOME}/cmdline-tools/latest/bin:${ANDROID_HOME}/platform-tools:${ANDROID_HOME}/emulator:${HOME}/axmol/tools/cmdline:${PATH}"

print_usage() {
    echo "Usage: ./build_android.sh [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -a, --arch ARCH       Target architecture: arm64 (default), x64, armv7, or 'arm64;x64'"
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

if [[ ! -d "${ANDROID_HOME}" ]]; then
    echo "Error: ANDROID_HOME (${ANDROID_HOME}) not found!"
    exit 1
fi

echo "==> Building PocketDash Android (Arch: ${ARCH}, Type: ${BUILD_TYPE})..."

BUILD_ARGS=("-p" "android" "-a" "${ARCH}")

if [[ ${TARGET_AAB} -eq 1 ]]; then
    BUILD_ARGS+=("-aab")
fi

if [[ "${BUILD_TYPE}" == "Release" ]]; then
    BUILD_ARGS+=("-xb" "--config" "Release")
fi

chmod +x proj.android/gradlew

# Run Axmol build command
axmol build "${BUILD_ARGS[@]}"

# Locate resulting APK
APK_PATH=$(find "${ANDROID_DIR}/app/build/outputs/apk" -name "*.apk" -type f | grep -i "${BUILD_TYPE}" | head -n 1)
if [[ -z "${APK_PATH}" ]]; then
    APK_PATH=$(find "${ANDROID_DIR}/app/build/outputs/apk" -name "*.apk" -type f | head -n 1)
fi

if [[ -n "${APK_PATH}" && -f "${APK_PATH}" ]]; then
    echo ""
    echo "================================================="
    echo "Android build succeeded! APK:"
    echo "  ${APK_PATH}"
    echo "================================================="
    
    if [[ ${INSTALL_DEVICE} -eq 1 ]]; then
        if command -v adb >/dev/null 2>&1; then
            echo "==> Installing APK to connected device/emulator..."
            adb install -r "${APK_PATH}"
            echo "==> Launching PocketDash..."
            adb shell am start -n "com.poly9a.pocketdash/dev.axmol.app.AppActivity"
        else
            echo "Warning: adb not found in PATH, skipping install."
        fi
    fi
else
    echo "Warning: Build finished, but APK was not found in ${ANDROID_DIR}/app/build/outputs/apk"
fi
