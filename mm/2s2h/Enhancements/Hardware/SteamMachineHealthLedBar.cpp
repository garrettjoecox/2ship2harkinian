#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "functions.h"
}

#define CVAR_NAME "gEnhancements.Hardware.SteamMachineHealthLedBar"
#define CVAR CVarGetInteger(CVAR_NAME, 1)

#if defined(__linux__)

#include <algorithm>
#include <cstdio>
#include <string>

// The Steam Machine's front LED bar is 17 individually-addressable RGB LEDs exposed by the kernel's
// multicolor LED class at /sys/class/leds/valve-leds[0..16]/multi_intensity ("R G B", 0-255 each).
// valve-leds[0] is the rightmost LED when facing the machine; index increases toward the left.
// Confirmed by direct testing on hardware - not documented by Valve as of SteamOS 3.8.
namespace {

constexpr int LED_COUNT = 17;
// The idle color SteamOS itself leaves the bar in before we touch it.
constexpr int IDLE_R = 1;
constexpr int IDLE_G = 90;
constexpr int IDLE_B = 255;

// Shared (not function-local) so RestoreIdle() can invalidate the cache: OnPlayDestroy fires on
// ordinary scene transitions too, not just quitting to file select, so it can overwrite every LED
// with the idle color mid-playthrough. Without invalidating here, the next Update() would see an
// unchanged health/flash state and skip repainting, leaving the bar stuck on the idle color.
int sLastLitCount = -1;
bool sLastFlashOn = true;

bool SteamMachineLedBarWrite(int index, int r, int g, int b) {
    std::string path = "/sys/class/leds/valve-leds[" + std::to_string(index) + "]/multi_intensity";
    FILE* file = fopen(path.c_str(), "w");
    if (file == nullptr) {
        return false;
    }
    fprintf(file, "%d %d %d\n", r, g, b);
    fclose(file);
    return true;
}

bool SteamMachineLedBarPresent() {
    static bool checked = false;
    static bool present = false;
    if (!checked) {
        FILE* file = fopen("/sys/class/leds/valve-leds[0]/multi_intensity", "r");
        present = file != nullptr;
        if (file != nullptr) {
            fclose(file);
        }
        checked = true;
    }
    return present;
}

void SteamMachineHealthLedBar_Update() {
    if (!SteamMachineLedBarPresent()) {
        return;
    }

    s16 health = gSaveContext.save.saveInfo.playerData.health;
    s16 healthCapacity = gSaveContext.save.saveInfo.playerData.healthCapacity;
    float fraction = std::clamp(static_cast<float>(health) / static_cast<float>(healthCapacity), 0.0f, 1.0f);
    int litCount = static_cast<int>(fraction * LED_COUNT + 0.5f);

    // While critical, flash the whole bar in sync with the low-HP alarm instead of showing a steady partial
    // fill. interfaceCtx.lifeSizeChangeDirection is the same growing(0)/shrinking(1) heartbeat-pulse phase
    // flag that LifeMeter_UpdateSizeAndBeep (z_lifemeter.c) uses to time NA_SE_SY_HITPOINT_ALARM - the alarm
    // fires exactly as the phase flips from shrinking back to growing, so mapping growing -> lit and
    // shrinking -> blackout keeps the flash locked to the beep without a separate timer.
    bool flashOn = true;
    if (LifeMeter_IsCritical()) {
        flashOn = gPlayState->interfaceCtx.lifeSizeChangeDirection == 0;
    }

    if (litCount == sLastLitCount && flashOn == sLastFlashOn) {
        return;
    }
    sLastLitCount = litCount;
    sLastFlashOn = flashOn;

    // valve-leds[0] is the rightmost LED (index increases toward the left). Lighting the highest
    // indices first makes the bar fill leftmost-first / drain rightmost-first as health rises and falls.
    for (int i = 0; i < LED_COUNT; i++) {
        if (flashOn && i >= LED_COUNT - litCount) {
            SteamMachineLedBarWrite(i, 255, 0, 0);
        } else {
            SteamMachineLedBarWrite(i, 0, 0, 0);
        }
    }
}

void SteamMachineHealthLedBar_RestoreIdle() {
    if (!SteamMachineLedBarPresent()) {
        return;
    }
    for (int i = 0; i < LED_COUNT; i++) {
        SteamMachineLedBarWrite(i, IDLE_R, IDLE_G, IDLE_B);
    }
    // Force the next Update() to repaint unconditionally, since we just overwrote every LED here.
    sLastLitCount = -1;
}

} // namespace

#endif // __linux__

static void RegisterSteamMachineHealthLedBar() {
#if defined(__linux__)
    COND_HOOK(OnPlayDrawWorldEnd, CVAR, []() { SteamMachineHealthLedBar_Update(); });
    COND_HOOK(OnPlayDestroy, CVAR, []() { SteamMachineHealthLedBar_RestoreIdle(); });
#endif
}

static RegisterShipInitFunc initFunc(RegisterSteamMachineHealthLedBar, { CVAR_NAME });
