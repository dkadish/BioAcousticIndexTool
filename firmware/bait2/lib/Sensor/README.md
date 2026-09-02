# Sensor Library

Base classes that define the measurement lifecycle shared by every sensor in BAIT2.

**Source**: `lib/Sensor/`

---

## Classes

### `Sensor`

**Header**: `include/Sensor.h`  
**Source**: `src/Sensor.cpp`

Abstract base class for a periodic sensor. Subclasses implement `process()`, `record()`,
and optionally `debug()`. The `loop()` method checks two independent `Chrono` timers:

1. **Measurement timer** (`_interval`) — triggers `process()` then `record()` when elapsed.
2. **Debug timer** (`m_debugInterval`) — triggers `debug()` for periodic log output.

#### Constructor

```cpp
Sensor(int interval, int debugInterval = 0);
Sensor(int interval, const char *filepath, int debugInterval = 0);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `interval` | `int` | Seconds between each `process()` / `record()` call |
| `filepath` | `const char *` | Path on the SD card where data are written (e.g. `"/rms.csv"`) |
| `debugInterval` | `int` | Seconds between `debug()` calls. `0` disables debug output |

#### Virtual methods

| Method | Default | Description |
|--------|---------|-------------|
| `setup()` | Restarts chronometers | Called once from Arduino `setup()`. Subclasses should call `Sensor::setup()` first |
| `loop()` | Checks timers | Called every Arduino loop iteration. Drives `process()`, `record()`, and `debug()` |
| `start()` | Restarts chronometers | Alias for resetting timers without subclass overrides |
| `reset()` | Restarts chronometers | Called after a recording cycle to restart timers. Subclasses call `Sensor::reset()` then zero their accumulators |
| `process()` | No-op | Read / aggregate raw values from hardware |
| `record()` | No-op | Persist averaged values to SD card and queue a LoRaWAN payload |
| `debug()` | No-op | Print a diagnostic summary to `Serial` |

#### Helper methods

```cpp
int   getInterval();                        // Returns _interval in seconds
int   firstMeasurement();                   // Seconds until the next aligned measurement
const char *getFilePath();                  // Returns the SD card filepath
int   getStatus();                          // Returns m_status (0 = OK)
void  writeTimestamp(stream_t *f);          // Writes "YYYY-M-D H:MM:SS, " to an open file
```

#### Static helper

```cpp
static int timeToFirstMeasurement(int interval);
```

Computes `interval - (now() % interval)` — the number of seconds until the next
interval boundary, given the current RTC time.

---

### `OversamplingSensor`

**Header**: `include/OversamplingSensor.h`  
**Source**: `src/OversamplingSensor.cpp`

Extends `Sensor` with a second, faster timer so `sample()` is called more frequently
than `record()`. This enables averaging many hardware readings over the reporting period.

#### Constructor

```cpp
OversamplingSensor(int interval, int measureInterval, int debugInterval = 0);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `interval` | `int` | Seconds between `record()` calls (inherited from `Sensor`) |
| `measureInterval` | `int` | Seconds between `sample()` calls. `0` samples as fast as possible |
| `debugInterval` | `int` | Seconds between `debug()` calls |

#### Virtual methods

| Method | Default | Description |
|--------|---------|-------------|
| `loop()` | Calls `Sensor::loop()` then checks `measureInterval` | Orchestrates both timers |
| `sample()` | Returns `true` | Read a single hardware measurement. Return `true` if a sample was taken, `false` to retry immediately |

---

## Usage Example

```cpp
class MySensor : public OversamplingSensor {
public:
    MySensor() : OversamplingSensor(/*interval=*/60, /*measureInterval=*/5) {}

    void setup() override {
        Sensor::setup();
        // initialise hardware...
    }

    bool sample() override {
        // read hardware, accumulate into sum_
        sum_ += readHardware();
        count_++;
        return true;
    }

    void record() override {
        float avg = count_ > 0 ? sum_ / count_ : 0.0f;
        reset();
        // write avg to SD, send over LoRaWAN...
    }

    void reset() override {
        sum_ = 0.0f;
        count_ = 0;
    }

private:
    float sum_ = 0.0f;
    int   count_ = 0;
};
```

In `main.cpp`:

```cpp
MySensor sensor;

void setup() { sensor.setup(); }
void loop()  { sensor.loop(); }
```
