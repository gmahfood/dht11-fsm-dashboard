#include <Arduino.h>
#include "Sensor.h"
#include "AlertSystem.h"

// =============================================================
//  Pin Configuration
// =============================================================
constexpr uint8_t DHT_PIN  = 2;   // DHT11 DATA → Arduino D2
constexpr uint8_t LED_PIN  = 13;  // Built-in LED for CRITICAL alert

// =============================================================
//  Timing
// =============================================================
constexpr uint32_t READ_INTERVAL_MS  = 2000;  // DHT11 needs ≥2s between reads
constexpr uint32_t PRINT_INTERVAL_MS = 5000;  // Print summary every 5s

// =============================================================
//  Thresholds — adjust these for your environment
// =============================================================
constexpr Thresholds THRESHOLDS = {
    .tempWarning  = 28.0f,
    .tempCritical = 35.0f,
    .humWarning   = 70.0f,
    .humCritical  = 85.0f
};

// =============================================================
//  Objects — created once, reused every loop
// =============================================================
Sensor      sensor(DHT_PIN, DHT11);
AlertSystem alertSystem(LED_PIN, THRESHOLDS);

// =============================================================
//  Timers — millis()-based, no blocking delay()
// =============================================================
uint32_t lastReadMs  = 0;
uint32_t lastPrintMs = 0;

// =============================================================
//  setup()
// =============================================================
void setup() {
    Serial.begin(9600);
    while (!Serial) {}   // Wait for serial port on some boards

    sensor.begin();
    alertSystem.begin();

    Serial.println(F("=== Embedded Sensor Dashboard ==="));
    Serial.println(F("Temp(C) | Humidity(%) | State"));
    Serial.println(F("---------------------------------"));
}

// =============================================================
//  loop()
// =============================================================
void loop() {
    uint32_t now = millis();

    // --- Read sensor every READ_INTERVAL_MS ---
    if (now - lastReadMs >= READ_INTERVAL_MS) {
        lastReadMs = now;

        if (sensor.read()) {
            alertSystem.update(sensor.temperature(), sensor.humidity());
        } else {
            Serial.println(F("[WARN] Sensor read failed — check wiring"));
        }
    }

    // --- Print summary every PRINT_INTERVAL_MS ---
    if (now - lastPrintMs >= PRINT_INTERVAL_MS) {
        lastPrintMs = now;

        if (sensor.isReady()) {
            Serial.print(F("Temp: "));
            Serial.print(sensor.temperature(), 1);
            Serial.print(F("°C  |  Hum: "));
            Serial.print(sensor.humidity(), 1);
            Serial.print(F("%  |  State: "));
            Serial.println(alertSystem.stateLabel());
        }
    }
}
