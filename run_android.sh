#!/usr/bin/env bash
set -e

# PocketDash Android Run & Emulator Script
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export ANDROID_HOME="${ANDROID_HOME:-$HOME/Android/Sdk}"
export ANDROID_SDK_ROOT="${ANDROID_SDK_ROOT:-$ANDROID_HOME}"
export ANDROID_NDK_HOME="${ANDROID_NDK_HOME:-$ANDROID_HOME/ndk/27.3.13750724}"
export ANDROID_NDK_ROOT="${ANDROID_NDK_ROOT:-$ANDROID_NDK_HOME}"
export PATH="/usr/bin:${ANDROID_HOME}/cmdline-tools/latest/bin:${ANDROID_HOME}/platform-tools:${ANDROID_HOME}/emulator:${HOME}/axmol/tools/cmdline:${PATH}"

AVD_NAME="PocketDash_Pixel"

print_usage() {
    echo "Usage: ./run_android.sh [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  --no-emulator    Do not launch emulator if no device is connected"
    echo "  --headless       Launch emulator in headless mode (no GUI window)"
    echo "  -h, --help       Show this help message"
}

LAUNCH_EMULATOR=1
EMULATOR_GUI=1

while [[ $# -gt 0 ]]; do
    case "$1" in
        --no-emulator)
            LAUNCH_EMULATOR=0
            shift
            ;;
        --headless)
            EMULATOR_GUI=0
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

# 1. Check if an Android device or emulator is running
CONNECTED_DEVICE=$(adb devices 2>/dev/null | grep -v "List" | grep "device$" | head -n 1 | awk '{print $1}')

if [[ -z "${CONNECTED_DEVICE}" ]]; then
    if [[ ${LAUNCH_EMULATOR} -eq 1 ]]; then
        echo "==> No active Android device/emulator found. Starting AVD: ${AVD_NAME}..."
        
        EMULATOR_BIN="${ANDROID_HOME}/emulator/emulator"
        if [[ ! -x "${EMULATOR_BIN}" ]]; then
            echo "Error: Android emulator binary not found at ${EMULATOR_BIN}"
            exit 1
        fi
        
        EMU_FLAGS=("-avd" "${AVD_NAME}" "-no-snapshot-load" "-gpu" "host" "-no-boot-anim")
        if [[ ${EMULATOR_GUI} -eq 0 ]]; then
            EMU_FLAGS+=("-no-window")
        fi
        
        # Start emulator in background
        "${EMULATOR_BIN}" "${EMU_FLAGS[@]}" > /dev/null 2>&1 &
        EMU_PID=$!
        echo "==> Emulator process started (PID: ${EMU_PID}). Waiting for device to come online..."
        
        adb wait-for-device
        
        echo "==> Waiting for Android OS to finish booting..."
        while [ "$(adb shell getprop sys.boot_completed 2>/dev/null | tr -d '\r')" != "1" ]; do
            sleep 1
        done
        echo "==> Android emulator is fully booted and ready!"
        CONNECTED_DEVICE=$(adb devices 2>/dev/null | grep -v "List" | grep "device$" | head -n 1 | awk '{print $1}')
    else
        echo "Error: No connected Android device found and --no-emulator was specified."
        exit 1
    fi
else
    echo "==> Found active Android device: ${CONNECTED_DEVICE}"
fi

# 2. Detect device CPU architecture
DEVICE_ABI=$(adb -s "${CONNECTED_DEVICE}" shell getprop ro.product.cpu.abi 2>/dev/null | tr -d '\r')
echo "==> Connected device ABI: ${DEVICE_ABI}"

BUILD_ARCH="arm64"
if [[ "${DEVICE_ABI}" == *"x86_64"* || "${DEVICE_ABI}" == *"x86"* ]]; then
    BUILD_ARCH="x64"
    echo "==> Targeting native x64 build for high-performance emulator execution."
else
    BUILD_ARCH="arm64"
    echo "==> Targeting arm64 build for ARM device."
fi

# 3. Build & Install PocketDash
echo "==> Building and installing PocketDash for ${BUILD_ARCH}..."
./build_android.sh -a "${BUILD_ARCH}" --install

echo ""
echo "================================================="
echo " PocketDash is now running on Android!           "
echo "================================================="
echo "Logs from device:"
adb logcat -s PocketDash:V axmol:V AndroidRuntime:E
