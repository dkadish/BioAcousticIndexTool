# Logging Library

Compile-time, verbosity-gated debug macros for the BAIT2 firmware.

**Header**: `lib/Logging/include/logging.h`

---

## Overview

All logging is done through preprocessor macros that expand to `Serial.printf()` calls (or
to nothing at all) depending on the `VERBOSITY` level defined at compile time. There is no
runtime overhead when a message's level exceeds `VERBOSITY`.

The global `VERBOSITY` for firmware builds is set to `4` in
`include/baitconfig.h`, which overrides the default value of `0` in `logging.h`.

---

## Verbosity Levels

| Level | Macro | When to use |
|-------|-------|-------------|
| 1 | `ERROR` | Unrecoverable failures (hardware not found, write error) |
| 2 | `WARNING` | Recoverable problems (no data to record, sensor not ready) |
| 3 | `INFO` | Noteworthy operational events |
| 4 | `DEBUG` | Frequent diagnostic messages (includes millisecond timestamp prefix) |
| 5 | `TRACE` | High-frequency / low-level tracing |

Compile with `-D VERBOSITY=<n>` (or set it in `baitconfig.h`) to enable all levels up to
and including `n`.

---

## Macro Reference

All macros accept a `printf`-style format string and optional variadic arguments.

### `ERROR(msg, ...)`

Active when `VERBOSITY >= 1`.

```cpp
ERROR("SD write failed on %s", filename);
```

### `WARNING(msg, ...)`

Active when `VERBOSITY >= 2`.

```cpp
WARNING("BME680 reading not ready, skipping");
```

### `INFO(msg, ...)`

Active when `VERBOSITY >= 3`.

```cpp
INFO("Sensor interval set to %d s", interval);
```

### `DEBUG(msg, ...)`

Active when `VERBOSITY >= 4`. Automatically prepends `"(<millis>) > "` to each message,
making it easy to correlate log lines with the system clock.

```cpp
DEBUG("ACI: %f, count: %d", aci_value, window_count);
// Output:  (12345) > ACI: 0.832100, count: 5
```

### `TRACE(msg, ...)`

Active when `VERBOSITY >= 5`.

```cpp
TRACE("FFT bin %d = %f", i, fft[i]);
```

---

## Configuration

Set `VERBOSITY` in one of the following ways (in order of precedence):

1. **Build flag** (recommended for environment-specific control):
   ```ini
   build_flags = -D VERBOSITY=3
   ```

2. **`include/baitconfig.h`** (project-wide default):
   ```cpp
   #define VERBOSITY 4
   ```

3. **`lib/Logging/include/logging.h`** (library default, currently `0`):
   ```cpp
   #define VERBOSITY 0
   ```

> **Note**: `baitconfig.h` is included early in `main.cpp` and re-defines `VERBOSITY`
> to `4`, overriding the library default. This means debug output is ON by default in
> firmware builds and should be reduced (e.g. `VERBOSITY=2`) for field deployments.
