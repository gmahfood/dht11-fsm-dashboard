#pragma once

#include <Arduino.h>
#include <DHT.h>
#include "CircularBuffer.h"

// =============================================================
//  Sensor
//  Wraps the DHT11 and keeps a rolling history for smoothing.
//  Uses a CircularBuffer of 5 readings to filter out noise.
// =============================================================
class Sensor {
public:
    static constexpr uint8_t HISTORY_SIZE = 5;

    // pin    — Arduino digital pin connected to DHT DATA line
    // dhtType — DHT11 or DHT22
    Sensor(uint8_t pin, uint8_t dhtType = DHT11);

    void begin();           // Call once in setup()
    bool read();            // Returns true if reading was valid

    float temperature()         const;  // Smoothed temperature (°C)
    float humidity()            const;  // Smoothed humidity (%)
    float rawTemperature()      const;  // Last raw reading
    float rawHumidity()         const;

    bool  isReady()             const { return ready_; }

private:
    DHT     dht_;
    bool    ready_;
    float   rawTemp_;
    float   rawHum_;

    CircularBuffer<float, HISTORY_SIZE> tempHistory_;
    CircularBuffer<float, HISTORY_SIZE> humHistory_;
};
