#include "AlertSystem.h"

// Constructor initializes LED pin, thresholds, and default state to OK.
// Safe default: always boot into OK state, never assume a fault at startup.
AlertSystem::AlertSystem(uint8_t ledPin, Thresholds thresholds)
    : ledPin_(ledPin), thresholds_(thresholds), state_(AlertState::OK)
{}

// Called once in setup(). Configures the LED pin and ensures it starts OFF.
// Always sync hardware state with software state at boot.
void AlertSystem::begin() {
    pinMode(ledPin_, OUTPUT);      // declare pin as output (sending voltage out)
    digitalWrite(ledPin_, LOW);    // ensure LED is off at startup
}

// Called every loop(). Evaluates current readings and transitions state if needed.
// Only calls applyState() when state actually changes, avoiding unnecessary hardware writes.
void AlertSystem::update(float temperature, float humidity) {
    AlertState newState = evaluate(temperature, humidity);

    // Only act on a state change. This is the core FSM pattern.
    if (newState != state_) {
        applyState(newState);
    }
}

// Pure logic function with no side effects and no hardware interaction.
// Evaluates sensor readings against thresholds and returns the appropriate state.
// CRITICAL is checked first because the most severe condition takes priority.
AlertState AlertSystem::evaluate(float temp, float hum) const {

    // Either reading exceeding the critical threshold triggers CRITICAL state.
    if (temp >= thresholds_.tempCritical || hum >= thresholds_.humCritical) {
        return AlertState::CRITICAL;
    }

    // Either reading exceeding the warning threshold triggers WARNING state.
    if (temp >= thresholds_.tempWarning || hum >= thresholds_.humWarning) {
        return AlertState::WARNING;
    }

    // All readings are within safe range.
    return AlertState::OK;
}

// Applies a new state by updating the stored state and driving hardware accordingly.
// The LED is only turned ON for CRITICAL. WARNING is communicated via serial only.
void AlertSystem::applyState(AlertState newState) {
    state_ = newState;    // update stored state first

    // If CRITICAL write HIGH (5V, LED on), otherwise write LOW (0V, LED off).
    digitalWrite(ledPin_, state_ == AlertState::CRITICAL ? HIGH : LOW);
}

// Returns a human readable string for the current state.
// Used for serial monitor output. const char* points to string literals in flash memory.
const char* AlertSystem::stateLabel() const {
    switch (state_) {
        case AlertState::OK:       return "OK";
        case AlertState::WARNING:  return "WARNING";
        case AlertState::CRITICAL: return "CRITICAL";
        default:                   return "UNKNOWN";  // defensive, handles unexpected values
    }
}