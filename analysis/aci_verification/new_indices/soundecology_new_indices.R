# Reference values for the new BAIT indices, computed with soundecology and seewave.
#
# - Bioacoustic Index: soundecology::bioacoustic_index, with fft_w = 256 to match
#   BAIT's 256-point FFT (defaults otherwise: 2000-8000 Hz).
# - Total entropy: seewave::H (Ht * Hf), plus its two factors: Hf = sh(meanspec())
#   and Ht = th(env(envt = "hil")), all with wl = 256.
# - Acoustic event count: neither package implements one, so there is no
#   published reference. It is computed here independently, as a two-pass count
#   over the seewave spectrogram, to check the firmware's single-pass version.
#
# Run from this directory: Rscript soundecology_new_indices.R
# Writes soundecology_new_indices.csv.

suppressPackageStartupMessages({
  library(tuneR)
  library(seewave)
})

# soundecology pulls in a long dependency chain (oce, vegan, ...). If it is not
# installed, set SOUNDECOLOGY_SRC to a checkout of its source (e.g.
# https://github.com/cran/soundecology) to load bioacoustic_index() on its own.
if (requireNamespace("soundecology", quietly = TRUE)) {
  bioacoustic_index <- soundecology::bioacoustic_index
} else {
  source(file.path(Sys.getenv("SOUNDECOLOGY_SRC"), "R", "bioacoust_index.R"))
}

fft_w <- 256

# Same definition as ecoacoustics::AcousticEventAccumulator (EcoacousticMath.h):
# band 1000-8000 Hz selected like soundecology, 0.5 dB histogram from -120 dB,
# background = lower edge of the modal bin, event = rise to >= background + 3 dB.
event_count <- function(wave, min_freq = 1000, max_freq = 8000, threshold_db = 3,
                        min_db = -120, step_db = 0.5, n_levels = 240) {
  f <- wave@samp.rate
  # Unnormalised spectro() gives 2|X|/wl on the raw samples. Dividing by full
  # scale puts it on the same scale as the Teensy's AudioAnalyzeFFT256 output,
  # which matters here because the level histogram has fixed dB limits.
  full_scale <- 2^(wave@bit - 1) - 1
  amp <- spectro(wave, f = f, wl = fft_w, plot = FALSE, dB = NULL, norm = FALSE)$amp / full_scale
  rows_width <- nrow(amp) / (f / 2)
  rows <- (min_freq * rows_width):(max_freq * rows_width)
  power <- colSums(amp[rows, , drop = FALSE]^2)
  db <- 10 * log10(pmax(power, 1e-30))
  level <- pmin(pmax(floor((db - min_db) / step_db), 0), n_levels - 1)
  mode <- which.max(tabulate(level + 1, nbins = n_levels)) - 1
  threshold <- mode + round(threshold_db / step_db)
  n <- length(level)
  c(events = sum(level[-n] < threshold & level[-1] >= threshold),
    background_db = min_db + mode * step_db)
}

rows <- list()
for (i in 0:49) {
  soundfile_name <- paste0("../SOUND/", i, ".wav")
  wave <- readWave(soundfile_name)
  if (wave@stereo) wave <- mono(wave, "left")
  f <- wave@samp.rate

  invisible(capture.output(bi <- bioacoustic_index(wave, fft_w = fft_w)))
  hf <- sh(meanspec(wave, f = f, wl = fft_w, plot = FALSE))
  ht <- th(env(wave, f = f, envt = "hil", plot = FALSE))
  h <- H(wave, f = f, wl = fft_w, envt = "hil")
  ev <- event_count(wave)

  rows[[i + 1]] <- data.frame(
    file = paste0("SOUND/", i, ".wav"), file_number = i,
    bi = bi$left_area, hf = hf, ht = ht, h = h,
    events = ev[["events"]], background_db = ev[["background_db"]]
  )
  message(soundfile_name, ": BI ", bi$left_area, ", H ", h)
}

write.csv(do.call(rbind, rows), "soundecology_new_indices.csv", row.names = FALSE)
