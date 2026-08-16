# dht11-fsm-dashboard

> A real-time embedded sensor monitoring system built in C++ for the Arduino Uno R3.
> Implements a three-state alert classifier with hardware LED output, a compile-time circular
> buffer for noise filtering, a live 20x4 I2C LCD dashboard, and a fully modular library
> architecture with zero dynamic memory allocation.

![Platform](https://img.shields.io/badge/platform-Arduino%20Uno%20R3-00979D?logo=arduino)
![Language](https://img.shields.io/badge/language-C%2B%2B17-blue)
![Build](https://img.shields.io/badge/build-PlatformIO-orange)
![Memory](https://img.shields.io/badge/heap%20usage-zero-brightgreen)
![License](https://img.shields.io/badge/license-MIT-lightgrey)

---

## Overview

`dht11-fsm-dashboard` is a from-scratch embedded C++ project targeting the **Arduino Uno R3**
(ATmega328P, 8-bit AVR, 2KB SRAM, 32KB Flash). It reads temperature and humidity from a
**DHT11 sensor**, filters noisy readings using a **moving average over a fixed-size circular
buffer**, classifies them into a **three-level alert state** (OK / WARNING / CRITICAL) that
drives an LED, and displays live readings on a **20x4 I2C LCD**.

The project is intentionally structured as a **multi-library C++ codebase** rather than a
single-file Arduino sketch, mirroring how real firmware teams organize production code.

---

## Features

| Feature | Implementation |
|---|---|
| Sensor reading | DHT11 over single-wire GPIO (wraps the DHT driver library) |
| Bad-read rejection | `isnan` validation; failed reads never enter the average |
| Noise filtering | 5-sample moving average via `CircularBuffer<T, N>` |
| Alert logic | 3-level classifier: `OK` / `WARNING` / `CRITICAL` |
| Hardware output | LED driven HIGH on `CRITICAL` state |
| Live display | 20x4 I2C LCD dashboard, refreshed every 2s |
| Timing | Non-blocking `millis()` pattern, no `delay()` in the loop |
| Serial output | Structured UART at 9600 baud |
| Memory safety | Zero heap allocation, no `new` / `delete` / `malloc` |
| Build system | PlatformIO with C++17 flags |
| Modularity | 3 decoupled libraries: SensorLib, AlertLib, BufferLib |

---

## System Architecture

```
                         main.cpp
        setup() / loop() — millis() timing, orchestration
        reads sensor, updates alert state, drives LCD + serial
          |            |               |              |
   ┌──────▼─────┐ ┌────▼──────┐  ┌─────▼──────┐  ┌────▼─────────┐
   │  SensorLib │ │  AlertLib │  │  20x4 LCD  │  │ USB Serial   │
   │ Sensor.*   │ │ AlertSystem│ │ (I2C 0x27) │  │ 9600 baud    │
   │ DHT11 wrap │ │ OK/WARN/   │ │ live temp, │  │ summary/5s   │
   │ + isnan    │ │ CRITICAL   │ │ hum, state │  └──────────────┘
   │ validation │ │ + LED      │  └────────────┘
   └──────┬─────┘ └───────────┘
          │
   ┌──────▼───────────────┐
   │      BufferLib       │
   │  CircularBuffer<T,N> │
   │  fixed-size ring,     │
   │  compile-time size,   │
   │  O(1) push, no heap   │
   └──────────────────────┘

Hardware
────────
DHT11 ──── D2 (GPIO)        reads every 2000ms, isnan-validated
LED   ──── D13 (GPIO)       driven HIGH on CRITICAL state
LCD   ──── I2C (0x27)       20x4, refreshed every 2000ms
UART  ──── USB Serial       summary printed every 5000ms
```

Note on the alert logic: the `AlertSystem` evaluates the target state purely from the current
readings (checking CRITICAL first for severity priority) and applies a change only on a
transition, using the stored state to avoid redundant hardware writes. It is a stateless
threshold classifier that tracks its state, not a transition-gated FSM — a deliberate choice
so the system always reflects live data and cannot get stuck in a stale state.

---

## Key C++ Concepts

### Circular Buffer Template
`CircularBuffer<T, N>` is a header-only, fully generic ring buffer with compile-time capacity
and **no dynamic memory** — the internal array is a fixed-size member sized by the template
parameter `N`. O(1) push (overwrites the oldest slot, advances a head pointer — nothing shifts),
O(N) average. `operator[]` exposes logical indexing (0 = oldest).

### Alert Classifier
`AlertSystem` maps live readings to `OK` / `WARNING` / `CRITICAL` using configurable thresholds,
checking the most severe condition first. `enum class AlertState` gives scoped, type-safe state
values. Only an actual state change drives the hardware.

### Non-Blocking Timing
No `delay()` in the loop. Each periodic task (sensor read, serial print, LCD refresh) tracks its
own last-run timestamp and fires when its interval elapses — cooperative time-slicing on a single
thread with no blocking.

---

## Hardware

Built with the SunFounder Inventor Lab Starter Kit (Arduino Uno R3, breadboard, jumpers, USB
cable) plus a WWZMDiB DHT11 module (built-in pull-up) and a 20x4 I2C LCD.

### Wiring

```
Arduino Uno R3        Device
──────────────        ──────
5V  ───────────────── DHT11 VCC   /  LCD VCC
GND ───────────────── DHT11 GND   /  LCD GND
D2  ───────────────── DHT11 DATA
A4 (SDA) ──────────── LCD SDA
A5 (SCL) ──────────── LCD SCL
D13 ───────────────── LED (built-in)

The DHT11 module has a built-in pull-up on DATA; no external resistor needed.
```

---

## PCB Design

Includes a custom 2-layer PCB designed in EasyEDA and fabricated by PCBWay (130x87mm, 1.6mm,
HASL, 1oz copper) — a first PCB design breaking out the Uno, DHT11, and LED alert circuit.
Gerber files are in `/hardware`.

---

## Project Structure

```
dht11-fsm-dashboard/
├── platformio.ini
├── src/
│   └── main.cpp                # setup(), loop(), millis() timing, LCD
├── lib/
│   ├── SensorLib/  Sensor.h/.cpp        # DHT11 wrap, isnan validation, buffering
│   ├── AlertLib/   AlertSystem.h/.cpp   # thresholds, states, LED output
│   └── BufferLib/  CircularBuffer.h     # header-only ring buffer template
├── hardware/                   # PCB gerbers
└── README.md
```

---

## Getting Started

Requires VS Code + PlatformIO. Clone, open, and PlatformIO auto-installs dependencies
(`DHT sensor library`, `Adafruit Unified Sensor`, `LiquidCrystal_I2C`). Connect the Uno via USB,
click Upload, then open the serial monitor at 9600 baud (`pio device monitor`).

To use a **DHT22** instead of DHT11, change one line in `src/main.cpp`:
```cpp
Sensor sensor(DHT_PIN, DHT22);
```

---

## Serial Output

```
=== Embedded Sensor Dashboard ===
Temp(C) | Humidity(%) | State
---------------------------------
Temp: 24.3°C  |  Hum: 55.2%  |  State: OK
Temp: 29.1°C  |  Hum: 71.3%  |  State: WARNING
Temp: 36.0°C  |  Hum: 86.2%  |  State: CRITICAL
```
Readings print every 5s; the sensor is sampled every 2s and averaged over the last 5 samples.

---

## Configuration

All tuneable values are `constexpr` constants in `main.cpp`:
```cpp
constexpr uint8_t  DHT_PIN           = 2;
constexpr uint8_t  LED_PIN           = 13;
constexpr uint32_t READ_INTERVAL_MS  = 2000;
constexpr uint32_t PRINT_INTERVAL_MS = 5000;
constexpr uint32_t LCD_INTERVAL_MS   = 2000;

constexpr Thresholds THRESHOLDS = {
    .tempWarning  = 28.0f,  .tempCritical = 35.0f,
    .humWarning   = 70.0f,  .humCritical  = 85.0f
};
```

---

## Roadmap

- [x] 20x4 I2C LCD dashboard (live temp, humidity, state)
- [x] Custom PCBWay PCB designed in EasyEDA *(first PCB design!)*
- [ ] SD card data logging with timestamps via SPI
- [ ] EEPROM persistence to save thresholds across power cycles
- [ ] Push-button to silence CRITICAL alert with debounce
- [ ] Second sensor input (soil moisture or light level)
- [ ] Custom 3D-printed enclosure (Bambu Lab P2S)
- [ ] Host-side unit tests with `googletest`

---

## Why This Project

Most Arduino tutorials produce single-file `.ino` sketches with global variables and `delay()`
calls. This project deliberately avoids that, writing C++ the way it would be written in a
professional embedded context: encapsulation over global state, templates for zero-cost generic
data structures, `enum class` over raw integers, `constexpr` over `#define`, non-blocking loops
over `delay()`, and modular libraries over monolithic sketches. The same patterns — ring buffers,
threshold classifiers, hardware abstraction classes, non-blocking scheduling — appear in
production firmware for medical devices, automotive ECUs, and robotics controllers.

---

## License

MIT — free to use, modify, and build on.
