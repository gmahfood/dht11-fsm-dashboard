#include "Sensor.h"

Sensor::Sensor(uint8_t pin, uint8_t dhtType)
    : dht_(pin, dhtType), ready_(false), rawTemp_(0.0f), rawHum_(0.0f)
{}

void Sensor::begin() {
    dht_.begin();
}

bool Sensor::read() {
    float t = dht_.readTemperature();
    float h = dht_.readHumidity();

    // isnan() catches failed reads from the DHT library
    if (isnan(t) || isnan(h)) {
        ready_ = false;
        return false;
    }

    rawTemp_ = t;
    rawHum_  = h;
    tempHistory_.push(t);
    humHistory_.push(h);
    ready_ = true;
    return true;
}

float Sensor::temperature()    const { return tempHistory_.average(); }
float Sensor::humidity()       const { return humHistory_.average(); }
float Sensor::rawTemperature() const { return rawTemp_; }
float Sensor::rawHumidity()    const { return rawHum_; }
