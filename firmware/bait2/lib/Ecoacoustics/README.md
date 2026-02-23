# Ecoacoustics Library

Real-time computation of ecoacoustic indices from live FFT audio data on the Teensy
MicroMod. Implements the Acoustic Complexity Index (ACI) along with supporting classes
for FFT reading, RMS measurement, and prototype Acoustic Difference / Diversity indices.

**Source**: `lib/Ecoacoustics/`  
**Library metadata**: `lib/Ecoacoustics/library.json`

---

## Classes

### `FFTReader`

**Header**: `include/FFTReader.h`  
**Source**: `src/FFTReader.cpp`

Wraps a Teensy `AudioAnalyzeFFT256` object to buffer the most recent FFT frame and
optionally record a full spectrogram to the SD card.

#### Constants

```cpp
static const int length = 128;
```

The number of usable frequency bins — half the FFT size (256 / 2 = 128). Each bin
covers approximately 172 Hz at a 44.1 kHz sample rate.

#### Constructor

```cpp
FFTReader(AudioAnalyzeFFT256 &fft, const char *filepath,
          bool recordSpectrogram = false, int averageTogether = 2,
          int debugInterval = 1);
```

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `fft` | `AudioAnalyzeFFT256 &` | — | Teensy Audio FFT analyser object |
| `filepath` | `const char *` | — | SD card path for spectrogram CSV (e.g. `"/fft.csv"`) |
| `recordSpectrogram` | `bool` | `false` | When `true`, each new frame is appended to the CSV file |
| `averageTogether` | `int` | `2` | Number of FFT frames averaged together by the analyser |
| `debugInterval` | `int` | `1` | Seconds between `debug()` log output |

#### Methods

```cpp
void setup() override;
```

Configures the `AudioAnalyzeFFT256` object: sets `averageTogether(_avg_together)` and
applies a Hamming window (`AudioWindowHamming256`). The window function reduces spectral
leakage at the cost of slightly reduced frequency resolution.

---

```cpp
void loop() override;
```

Checks `_fft.available()`. When a new frame is ready, copies all 128 bin values into
the internal buffer, increments the frame counter, and (if `recordSpectrogram` is `true`)
calls `recordFFT()`.

---

```cpp
float get(int i);
```

Returns the magnitude of bin `i` from the most recently captured frame.  
Valid indices: `0` to `FFTReader::length - 1` (0–127).

---

```cpp
bool available();
```

Returns `true` if the FFT produced a new frame during the last `loop()` call.

---

```cpp
long getCount() const;
```

Total number of FFT frames captured since construction or the last `resetCount()`.

---

```cpp
void resetCount();
```

Resets the frame counter to zero. Called by `ACI_TemporalWindow` at the end of each
temporal window.

---

```cpp
void recordFFT();
```

Writes one row to the spectrogram CSV file:
`timestamp, bin0, bin1, ..., bin127`

Only called when `recordSpectrogram == true`.

---

#### Spectrogram Buffer (conditional)

When compiled with `-D SAVE_SPECTROGRAM`, `FFTReader` maintains an in-memory spectrogram
array `_spectrogram[128][250]` and exposes:

```cpp
float getSpectro(int f, int t);  // Magnitude of frequency bin f at time step t
int   getMaxCount();             // Maximum number of time steps stored (250)
```

This feature is memory-intensive (~128 KB) and disabled by default.

---

### `ACI_TemporalWindow`

**Header**: `include/ACI_TemporalWindow.h`  
**Source**: `src/ACI_TemporalWindow.cpp`

Computes the Acoustic Complexity Index (ACI) over a sliding temporal window. The ACI
quantifies the variability of spectral energy across time steps:

```
ACI = Σ_k  (D_k / I_k)
```

where for each frequency bin `k`:
- `D_k` = sum of absolute differences between successive FFT frames
- `I_k` = sum of intensities across all frames

A higher ACI value indicates more complex, rapidly-changing sounds (e.g. bird song)
relative to monotone noise (e.g. a generator).

*Reference*: Farina et al. (2016). "The Application of Soundscape Ecology to the
Soundscape Approach." *Landscape Ecology.*

#### Constructor

```cpp
ACI_TemporalWindow(int interval, FFTReader &fft,
                   bool doFilter = false,
                   bool doDiscardAdjacentZeros = false,
                   int debugInterval = 0);
```

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `interval` | `int` | — | Seconds per temporal window. At the end of each window, `process()` accumulates the window's ACI contribution |
| `fft` | `FFTReader &` | — | FFT source to read bin values from |
| `doFilter` | `bool` | `false` | Zero FFT bins below `_filter_below` threshold before computing differences. See Farina et al. (2016) |
| `doDiscardAdjacentZeros` | `bool` | `false` | Ignore the difference between two frames when at least one of them is zero. See Farina et al. (2016) |
| `debugInterval` | `int` | `0` | Seconds between `debug()` output |

#### Methods

```cpp
void setup() override;
```

Calls `Sensor::setup()` and `reset()` to initialise the accumulation buffers.

---

```cpp
void loop() override;
```

Called every Arduino loop iteration. On each new FFT frame (when
`FFTReader::available()` is `true`):

1. First frame in a window: initialises `I_k[]` from the current frame.
2. Subsequent frames: computes `|frame_k - I_k_prev|` for each bin, accumulates
   into `D[]` and `I[]`, and optionally applies the filter and zero-discard rules.

At the end of the window interval, calls `process()`.

---

```cpp
void process() override;
```

Accumulates the per-window ACI contribution:

```
aci += Σ_k  D[k] / I[k]   (skipping bins where I[k] == 0)
```

Then resets `D[]`, `I[]`, and `I_k[]` for the next window and increments the window
count `_count`.

---

```cpp
void reset() override;
```

Clears all accumulation buffers and sets `_first = true` so the next frame
re-initialises `I_k`.

---

```cpp
float getValue();
```

Returns the running ACI sum accumulated across all windows since the last `zero()` call.
Passed to `AcousticComplexityIndex::record()` at the end of the measurement interval.

---

```cpp
int getCount();
```

Number of temporal windows processed since the last `zero()` call.

---

```cpp
int getFFTCount();
```

Delegates to `FFTReader::getCount()` — the number of individual FFT frames seen.

---

```cpp
void zero();
```

Resets `aci` to `0.0` and `_count` to `0`, and calls `FFTReader::resetCount()`.
Called by `AcousticComplexityIndex::reset()` after a recording cycle.

---

```cpp
void setFilter(bool doFilter);
void setFilterLimit(float filterLimit);
void setDiscardAdjacentZeros(bool doDiscardAdjacentZeros);
```

Runtime setters for the three algorithmic parameters. Their defaults are set in the
constructor and can be changed at any time.

---

### `AcousticComplexityIndex`

**Header**: `include/AcousticComplexityIndex.h`  
**Source**: `src/AcousticComplexityIndex.cpp`

Top-level recording sensor that owns an `ACI_TemporalWindow` and, at the end of a
longer measurement interval, samples the accumulated ACI value, writes it to the SD
card, and transmits it over LoRaWAN.

#### Constructor

```cpp
AcousticComplexityIndex(ACI_TemporalWindow &aci_tw, const char *filepath,
                        LoRaWANTTN *lorattn, int interval,
                        int debugInterval = 30);
```

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `aci_tw` | `ACI_TemporalWindow &` | — | The temporal window accumulator |
| `filepath` | `const char *` | — | SD card path for ACI CSV (e.g. `"/aci.csv"`) |
| `lorattn` | `LoRaWANTTN *` | — | LoRaWAN uplink handle |
| `interval` | `int` | — | Seconds between `record()` calls |
| `debugInterval` | `int` | `30` | Seconds between `debug()` log output |

#### Methods

```cpp
void reset() override;
```

Calls `Sensor::reset()` and `ACI_TemporalWindow::zero()` to clear all accumulated data.

---

```cpp
void record() override;
```

1. Reads the current ACI value from `aci_tw.getValue()`.
2. Calls `reset()` to zero accumulators for the next period.
3. Appends `aci` to the CayenneLPP buffer under `ACOUSTIC_COMPLEXITY_INDEX` (16) and
   marks the LoRaWAN instance dirty.
4. Opens the CSV file and appends a row: `timestamp, aci_value`.

---

```cpp
void debug() override;
```

Prints the current ACI value and window count to `Serial`.

---

#### CSV Format

```
2024-07-25 14:30:00, 0.832100
```

---

#### Typical Configuration in `main.cpp`

```cpp
// 256-point FFT, averaged over 2 frames, not recording spectrogram
FFTReader fftReader = FFTReader(fft256_l, "/fft.csv", false, 2, -1);

// ACI temporal window: 5-second sub-windows, no filter, no zero-discard
ACI_TemporalWindow aci_window = ACI_TemporalWindow(5, fftReader, false, false, 0);

// Record every 60 seconds (interval), with 30-second debug output
AcousticComplexityIndex aci = AcousticComplexityIndex(aci_window, "/aci.csv", &lora, interval, 60);
```

---

### `RootMeanSquare`

**Header**: `include/RMS.h`  
**Source**: `src/RMS.cpp`

Measures and records the Root Mean Square (RMS) amplitude of the audio signal.
Extends `OversamplingSensor` to average many RMS readings over the reporting interval.

#### Constructor

```cpp
RootMeanSquare(AudioAnalyzeRMS &rms, const char *filepath,
               LoRaWANTTN *lorattn, int interval = 60,
               int measureInterval = 0, int debugInterval = 30);
```

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `rms` | `AudioAnalyzeRMS &` | — | Teensy Audio RMS analyser object |
| `filepath` | `const char *` | — | SD card path for CSV output (e.g. `"/rms.csv"`) |
| `lorattn` | `LoRaWANTTN *` | — | LoRaWAN uplink handle |
| `interval` | `int` | `60` s | Seconds between `record()` calls |
| `measureInterval` | `int` | `0` | Seconds between `sample()` calls. `0` samples as fast as the audio engine provides new values |
| `debugInterval` | `int` | `30` | Seconds between `debug()` output |

#### Methods

```cpp
void setup() override;
```

Calls `Sensor::setup()` and logs a startup message.

---

```cpp
bool sample() override;
```

Checks `m_rms.available()`. If a new RMS value is ready, accumulates it into
`m_rms_accumulator` and increments `m_count`. Returns `true` if a sample was taken.

---

```cpp
void record() override;
```

Computes `currentRMS() * 100` (scales to a 0–100 integer range), resets accumulators,
then:
- Adds the scaled value to the CayenneLPP buffer under `ROOT_MEAN_SQUARE` (8).
- Marks LoRaWAN dirty.
- Writes `timestamp, value` to the CSV file.

---

```cpp
void debug() override;
```

Prints current RMS (scaled × 100), the raw accumulator, and the sample count.

---

```cpp
long getCount() const;
```

Returns the number of RMS samples accumulated in the current interval.

---

```cpp
void reset();
```

Zeroes `m_count` and `m_rms_accumulator`.

---

#### CSV Format

```
2024-07-25 14:30:00, 12
```

The value is `RMS * 100` truncated to an integer (e.g. `0.12` → `12`).

---

### `AcousticDifferenceIndex`

**Header**: `include/AcousticDifferenceIndex.h`  
**Source**: `src/AcousticDifferenceIndex.cpp`

Prototype implementation of an Acoustic Difference Index. Tracks cumulative per-bin
differences (`diff_freq`) and cumulative totals (`diff_cumfreq`) across time steps.
**Not currently wired into `main.cpp`.**

---

### `AcousticDiversityIndex`

**Header**: `include/AcousticDiversityIndex.h`  
**Source**: `src/AcousticDiversityIndex.cpp`

Prototype implementation of an Acoustic Diversity Index. Divides the spectrum into
10 bands of approximately 1 000 Hz each and computes a diversity measure over them.
**Not currently wired into `main.cpp`.**

---

## Compile-time Parameters

Defined in `include/parameters.h`:

| Macro | Value | Description |
|-------|-------|-------------|
| `PRECISION` | `8` | Decimal precision used when writing floating-point values to CSV files |

---

## Dependencies

| Library | Notes |
|---------|-------|
| `Audio` (Teensy) | `AudioAnalyzeFFT256`, `AudioAnalyzeRMS` — provided by the Teensy Audio library |
| `Sensor` (local) | Base class for all sensors |
