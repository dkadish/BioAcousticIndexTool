# New index verification (BI, Htf, event count)

Compares the firmware's Bioacoustic Index, total entropy (H = Ht × Hf) and acoustic event
count against `soundecology` / `seewave`, using the same 50 files in `../SOUND/` as the ACI
comparison. Results are in [`RESULTS.md`](RESULTS.md).

No board is needed: the WAV files are turned into FFT frames in Python and fed to
`replay_indices`, which is compiled from the firmware's own index code
(`firmware/bait2/lib/EcoacousticMath`). Two sets of frames are used:

- **reference frames**: built like seewave's `spectro` (256-point FFT, Hanning window, no
  overlap). Any difference from the reference here is a bug in the index maths.
- **Teensy-style frames**: built like `AudioAnalyzeFFT256` + `FFTReader` on the device (Hamming
  window, 50% overlap, 2 FFTs averaged). Differences here show how the device's FFT set-up moves
  the values. The Teensy's fixed-point FFT rounding is not modelled.

## Running

```bash
cd analysis/aci_verification/new_indices
python3 bait_new_indices.py          # -> bait_new_indices.csv (needs g++, numpy, scipy)
Rscript soundecology_new_indices.R   # -> soundecology_new_indices.csv (needs tuneR, seewave, soundecology)
python3 compare_new_indices.py       # -> RESULTS.md (needs pandas, scipy)
```

If `soundecology` itself will not install (it depends on `oce`, `vegan` and others), point
`SOUNDECOLOGY_SRC` at a checkout of its source, e.g. `https://github.com/cran/soundecology`, and
the script loads `bioacoustic_index()` from there.

## Summary (soundecology 1.3.3, seewave 2.2.4)

| Index | Reference frames vs. reference | Teensy-style frames vs. reference |
|---|---|---|
| Bioacoustic Index | identical (0.00%) | median 7.0%, max 27%, Spearman 0.85 |
| Spectral entropy Hf | identical (0.00%) | median 1.6%, max 3.7%, Spearman 0.997 |
| Temporal entropy Ht | median 0.3%, max 1.7% | median 0.3%, max 1.8% |
| Total entropy H | median 0.3%, max 1.7%, Spearman 0.998 | median 1.2%, max 3.5%, Spearman 0.996 |
| Event count | identical on 50/50 files | median 17%, max 60% lower, Spearman 0.68 |

Notes:

- **BI and Hf** match exactly on the same frames. The Teensy-style differences come from the
  window, the 50% overlap and the averaging of two FFTs in `FFTReader`.
- **Ht** differs slightly even on reference frames. seewave uses the Hilbert envelope of every
  sample, and the firmware only sees FFT frames, so its envelope has one value per frame
  (`sqrt(sum |X_k|^2)`).
- **Event count** has no soundecology or seewave function. The R script computes the same
  definition independently (histogram mode as background, rises to at least background + 3 dB,
  1–8 kHz band) as a two-pass count. This checks the firmware's single-pass version, not the
  definition itself. Averaging FFT frames on the device smooths the level and removes many
  short crossings. At around 10–20 events per second, the current definition counts frame-level
  fluctuations rather than distinct calls. The choice of threshold, smoothing and minimum
  duration belongs in #69.
