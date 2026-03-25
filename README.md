# Embedded Sensor Dashboard
### Arduino Uno R3 · C++17 · PlatformIO

A beginner-to-portfolio embedded C++ project that reads temperature and humidity
from a DHT11 sensor, applies a moving-average filter, and drives a finite state
machine alert system — all without dynamic memory allocation.

---

## Features

| Feature | Details |
|---|---|
| **Sensor reading** | DHT11 via Adafruit library |
| **Noise filtering** | 5-sample circular buffer moving average |
| **Alert FSM** | OK → WARNING → CRITICAL states |
| **Non-blocking timing** | `millis()`-based, no `delay()` |
| **UART output** | Structured data at 9600 baud |
| **Zero heap use** | All buffers fixed at compile time |

---

## Wiring

```
DHT11 Sensor        Arduino Uno R3
─────────────       ──────────────
VCC  ──────────────  5V
GND  ──────────────  GND
DATA ──────────────  D2

LED (alert)         Arduino Uno R3
─────────────       ──────────────
Built-in LED (D13) used for CRITICAL alert
```

---

## Project Structure

```
embedded-sensor-dashboard/
├── platformio.ini          # Build config, board, libraries
├── src/
│   └── main.cpp            # Entry point — setup() and loop()
├── lib/
│   ├── SensorLib/
│   │   ├── Sensor.h        # DHT11 wrapper with smoothing
│   │   └── Sensor.cpp
│   ├── AlertLib/
│   │   ├── AlertSystem.h   # Finite state machine (OK/WARNING/CRITICAL)
│   │   └── AlertSystem.cpp
│   └── BufferLib/
│       └── CircularBuffer.h  # Generic fixed-size ring buffer (template)
└── docs/
    └── wiring.md
```

---

## Getting Started

### 1. Install tools
- [VS Code](https://code.visualstudio.com/)
- [PlatformIO extension](https://platformio.org/install/ide?install=vscode)

### 2. Clone and open
```bash
git clone <your-repo-url>
cd embedded-sensor-dashboard
code .
```
PlatformIO will auto-install all library dependencies.

### 3. Upload
Connect your Arduino Uno via USB, then click **Upload** (→) in PlatformIO,
or run:
```bash
pio run --target upload
```

### 4. Monitor serial output
```bash
pio device monitor
```
You should see:
```
=== Embedded Sensor Dashboard ===
Temp(C) | Humidity(%) | State
---------------------------------
Temp: 24.3°C  |  Hum: 55.2%  |  State: OK
```

---

## Key C++ Concepts Demonstrated

- **Classes & encapsulation** — `Sensor`, `AlertSystem`, `CircularBuffer`
- **Templates** — `CircularBuffer<T, N>` with compile-time size
- **Enums** — `enum class AlertState` for type-safe FSM states
- **`constexpr`** — compile-time constants instead of `#define`
- **Non-blocking loops** — `millis()` pattern instead of `delay()`
- **No dynamic memory** — no `new`/`delete`, safe for embedded targets

---

## Extending This Project (ideas for later)

- [ ] Add an LCD display (I2C) to show readings without a PC
- [ ] Log readings to SD card with timestamps
- [ ] Add a second sensor (soil moisture, light level)
- [ ] Implement EEPROM persistence for thresholds
- [ ] Add a pushbutton to silence CRITICAL alerts

---

## License
MIT — free to use, modify, and share.
