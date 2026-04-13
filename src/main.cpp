#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
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
constexpr uint32_t LCD_INTERVAL_MS   = 2000;  // Refresh LCD every 2s

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
//  LCD — 20x4 I2C display
//  Common addresses: 0x27 or 0x3F
//  If display is blank, try swapping to the other address
// =============================================================
LiquidCrystal_I2C lcd(0x27, 20, 4);

// =============================================================
//  Timers — millis()-based, no blocking delay()
// =============================================================
uint32_t lastReadMs  = 0;
uint32_t lastPrintMs = 0;
uint32_t lastLcdMs   = 0;

// =============================================================
//  LCD Helper — formats and writes data to the 20x4 display
// =============================================================
//  Row 0: " BAREMETAL  LABS    "
//  Row 1: " Temp:  XX.X C     "
//  Row 2: " Hum:   XX.X %     "
//  Row 3: " State: XXXXXXX    "
// =============================================================
void updateLcd(float temp, float hum, const char* state) {
    // Row 0 — Project branding (static, but rewritten for simplicity)
    lcd.setCursor(0, 0);
    lcd.print(F(" BAREMETAL  LABS    "));

    // Row 1 — Temperature
    lcd.setCursor(0, 1);
    lcd.print(F(" Temp:  "));
    lcd.print(temp, 1);
    lcd.print(F(" C      "));

    // Row 2 — Humidity
    lcd.setCursor(0, 2);
    lcd.print(F(" Hum:   "));
    lcd.print(hum, 1);
    lcd.print(F(" %      "));

    // Row 3 — FSM State
    lcd.setCursor(0, 3);
    lcd.print(F(" State: "));
    lcd.print(state);
    lcd.print(F("            "));  // Clear remaining chars on the row
}

// =============================================================
//  setup()
// =============================================================
void setup() {
    Serial.begin(9600);
    while (!Serial) {}   // Wait for serial port on some boards

    sensor.begin();
    alertSystem.begin();

    // Initialize LCD
    lcd.init();
    lcd.backlight();

    // Startup splash screen
    lcd.setCursor(0, 0);
    lcd.print(F(" BAREMETAL  LABS    "));
    lcd.setCursor(0, 1);
    lcd.print(F("  Sensor Dashboard "));
    lcd.setCursor(0, 2);
    lcd.print(F("   Initializing... "));
    lcd.setCursor(0, 3);
    lcd.print(F("     v1.0          "));

    Serial.println(F("=== Embedded Sensor Dashboard ==="));
    Serial.println(F("Temp(C) | Humidity(%) | State"));
    Serial.println(F("---------------------------------"));

    delay(2000);  // Show splash for 2s, only blocking call in the project
    lcd.clear();
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

            // Show error on LCD
            lcd.setCursor(0, 3);
            lcd.print(F(" [WARN] CHECK WIRE  "));
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

    // --- Update LCD every LCD_INTERVAL_MS ---
    if (now - lastLcdMs >= LCD_INTERVAL_MS) {
        lastLcdMs = now;

        if (sensor.isReady()) {
            updateLcd(
                sensor.temperature(),
                sensor.humidity(),
                alertSystem.stateLabel()
            );
        }
    }
}