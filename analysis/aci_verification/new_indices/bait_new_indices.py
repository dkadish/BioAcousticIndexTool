#!/usr/bin/env python3
"""Compute the new BAIT indices (BI, Htf, event count) for the verification sound files.

The WAV files are turned into FFT frames in Python and the frames are fed to
``replay_indices``, which is built from the firmware's own index code
(``firmware/bait2/lib/EcoacousticMath``). Two front ends are used:

``reference``
    Frames built the way seewave/soundecology build them: 256-point FFT,
    symmetric Hanning window, no overlap, no averaging, on the same amplitude
    scale as the Teensy frames. Differences from soundecology here come from
    the index maths alone.

``teensy``
    Frames built the way ``AudioAnalyzeFFT256`` builds them on the device:
    Hamming window, a new FFT every 128 samples (50% overlap), magnitudes
    averaged over 2 FFTs (``FFTReader(..., averageTogether = 2)``), scaled by
    1/128. The fixed-point (q15) rounding of the Teensy FFT is not modelled.

Writes ``bait_new_indices.csv`` next to this script.
"""
import csv
import subprocess
import sys
from pathlib import Path

import numpy as np
from scipy.io import wavfile

HERE = Path(__file__).resolve().parent
SOUND = HERE.parent / "SOUND"
FIRMWARE_MATH = HERE.parents[2] / "firmware" / "bait2" / "lib" / "EcoacousticMath"
REPLAY = HERE / "replay_indices"

FFT_SIZE = 256
BINS = FFT_SIZE // 2


def build_replay():
    sources = [HERE / "replay_indices.cpp", FIRMWARE_MATH / "src" / "EcoacousticMath.cpp"]
    if REPLAY.exists() and all(REPLAY.stat().st_mtime > s.stat().st_mtime for s in sources):
        return
    subprocess.run(
        ["g++", "-O2", "-I", str(FIRMWARE_MATH / "include"), *map(str, sources), "-o", str(REPLAY)],
        check=True,
    )


def read_mono(path):
    rate, data = wavfile.read(path)
    if data.ndim > 1:
        data = data[:, 0]  # soundecology's *_left values
    if np.issubdtype(data.dtype, np.integer):
        data = data.astype(np.float64) / np.iinfo(data.dtype).max
    return rate, data.astype(np.float64)


def reference_frames(x):
    n = (len(x) // FFT_SIZE) * FFT_SIZE
    frames = x[:n].reshape(-1, FFT_SIZE) * np.hanning(FFT_SIZE)
    # Same scale as the Teensy frames (and seewave's 2|X|/wl on 16-bit samples)
    return np.abs(np.fft.fft(frames, axis=1))[:, :BINS] / 128.0


def teensy_frames(x, average_together=2):
    hop = FFT_SIZE // 2
    starts = np.arange(0, len(x) - FFT_SIZE + 1, hop)
    frames = np.stack([x[s:s + FFT_SIZE] for s in starts]) * np.hamming(FFT_SIZE)
    mags = np.abs(np.fft.fft(frames, axis=1))[:, :BINS] / 128.0
    n = (len(mags) // average_together) * average_together
    return mags[:n].reshape(-1, average_together, BINS).mean(axis=1)


def replay(frames, rate):
    out = subprocess.run(
        [str(REPLAY), str(rate)],
        input=frames.astype("<f4").tobytes(),
        capture_output=True,
        check=True,
    )
    return out.stdout.decode().strip().split(",")


def main():
    build_replay()
    fields = ["bi", "hf", "ht", "h", "events", "background_db", "frames"]
    rows = []
    for i in range(50):
        path = SOUND / f"{i}.wav"
        rate, x = read_mono(path)
        row = {"file": f"SOUND/{i}.wav", "file_number": i}
        for name, make in (("reference", reference_frames), ("teensy", teensy_frames)):
            values = replay(make(x), rate)
            row.update({f"{name}_{f}": v for f, v in zip(fields, values)})
        rows.append(row)
        print(f"{path.name}: BI {row['teensy_bi']}, H {row['teensy_h']}, events {row['teensy_events']}",
              file=sys.stderr)

    with open(HERE / "bait_new_indices.csv", "w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=list(rows[0]))
        writer.writeheader()
        writer.writerows(rows)


if __name__ == "__main__":
    main()
