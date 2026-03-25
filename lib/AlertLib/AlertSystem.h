#pragma once

#include <Arduino.h>

// =============================================================
//  AlertSystem — Finite State Machine
//
//  States:
//    OK       — all readings within safe range
//    WARNING  — one reading near the threshold
//    CRITICAL — threshold exceeded, LED on
//
//  This is a classic pattern used in real embedded products.
// =============================================================

enum class AlertState : uint8_t {
    OK,
    WARNING,
    CRITICAL
};

struct Thresholds {
    float tempWarning;   // e.g. 28.0°C
    float tempCritical;  // e.g. 35.0°C
    float humWarning;    // e.g. 70.0%
    float humCritical;   // e.g. 85.0%
};

class AlertSystem {
public:
    AlertSystem(uint8_t ledPin, Thresholds thresholds);

    void begin();                               // Call once in setup()
    void update(float temperature, float humidity); // Call each loop

    AlertState  state()       const { return state_; }
    const char* stateLabel()  const;            // "OK" / "WARNING" / "CRITICAL"

private:
    uint8_t    ledPin_;
    Thresholds thresholds_;
    AlertState state_;

    AlertState evaluate(float temp, float hum) const;
    void       applyState(AlertState newState);
};
