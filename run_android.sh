#!/usr/bin/env bash
set -e

# PocketDash Android Run & Emulator Script
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export ANDROID_HOME="${ANDROID_HOME:-$HOME/Android/Sdk}"
export ANDROID_SDK_ROOT="${ANDROID_SDK_ROOT:-$ANDROID_HOME}"
export ANDROID_AVD_HOME="${ANDROID_AVD_HOME:-$HOME/.config/.android/avd}"
export ANDROID_NDK_HOME="${ANDROID_NDK_HOME:-$ANDROID_HOME/ndk/27.3.13750724}"
export ANDROID_NDK_ROOT="${ANDROID_NDK_ROOT:-$ANDROID_NDK_HOME}"
export PATH="/usr/bin:${ANDROID_HOME}/cmdline-tools/latest/bin:${ANDROID_HOME}/platform-tools:${ANDROID_HOME}/emulator:${HOME}/axmol/tools/cmdline:${PATH}"

# Ensure ~/.android/avd symlink points to ~/.config/.android/avd
mkdir -p "$HOME/.android"
if [[ ! -e "$HOME/.android/avd" ]]; then
    ln -sfn "$HOME/.config/.android/avd" "$HOME/.android/avd"
fi

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
        
        # Start emulator in background, logging output
        "${EMULATOR_BIN}" "${EMU_FLAGS[@]}" > /tmp/pocketdash_emulator.log 2>&1 &
        EMU_PID=$!
        echo "==> Emulator process started (PID: ${EMU_PID}). Checking startup..."
        
        sleep 2
        if ! kill -0 $EMU_PID 2>/dev/null; then
            echo "Error: Emulator process exited prematurely! Log:"
            cat /tmp/pocketdash_emulator.log
            exit 1
        fi
        
        echo "==> Waiting for device to come online..."
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
GRADLE_ARCH="arm64-v8a"
if [[ "${DEVICE_ABI}" == *"x86_64"* || "${DEVICE_ABI}" == *"x86"* ]]; then
    BUILD_ARCH="x64"
    GRADLE_ARCH="x86_64"
    echo "==> Targeting native x64 build for high-performance emulator execution."
else
    BUILD_ARCH="arm64"
    GRADLE_ARCH="arm64-v8a"
    echo "==> Targeting arm64 build for ARM device."
fi

# 3. Build Debug APK using Gradle directly for fastest reliable build
echo "==> Building PocketDash Debug APK (${BUILD_ARCH})..."
cd "${PROJECT_DIR}/proj.android"
chmod +x ./gradlew
./gradlew assembleDebug -P__1K_ARCHS="${GRADLE_ARCH}"
cd "${PROJECT_DIR}"

# 4. Locate APK
APK_PATH=$(find "${PROJECT_DIR}/proj.android/app/build/outputs/apk/debug" -name "*.apk" -type f | head -n 1)
if [[ -z "${APK_PATH}" ]]; then
    APK_PATH=$(find "${PROJECT_DIR}/proj.android/app/build/outputs/apk" -name "*.apk" -type f | head -n 1)
fi

if [[ -z "${APK_PATH}" || ! -f "${APK_PATH}" ]]; then
    echo "Error: Could not locate built APK!"
    exit 1
fi

echo "==> Installing APK (${APK_PATH}) to ${CONNECTED_DEVICE}..."
adb -s "${CONNECTED_DEVICE}" install -r "${APK_PATH}"

echo "==> Launching PocketDash on ${CONNECTED_DEVICE}..."
adb -s "${CONNECTED_DEVICE}" shell am start -n "com.poly9a.pocketdash/dev.axmol.app.AppActivity"

echo ""
echo "================================================="
echo " PocketDash is now running on Android!           "
echo "================================================="
echo "Streaming logs (press Ctrl+C to stop log streaming):"
adb -s "${CONNECTED_DEVICE}" logcat -s PocketDash:V axmol:V AndroidRuntime:E
