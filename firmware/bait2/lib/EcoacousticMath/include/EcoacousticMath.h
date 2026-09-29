//
// Board-independent maths for the spectral acoustic indices.
//
// Nothing in this library depends on Arduino, the Teensy Audio library or the
// SD card, so it compiles on the desktop (`pio test -e desktop`). The Sensor
// classes in the Ecoacoustics library feed FFT frames into these accumulators
// and handle timing, logging and transmission.
//
// Every accumulator is streaming: frames are added one at a time with
// addFrame() and the index for everything seen since the last reset() is read
// with value(). Memory use is fixed and does not grow with the interval length.
//

#ifndef BAIT2_ECOACOUSTICMATH_H
#define BAIT2_ECOACOUSTICMATH_H

namespace ecoacoustics
{

    /** Number of magnitude bins in an FFT frame (AudioAnalyzeFFT256 gives 256 / 2 = 128). */
    const int kSpectrumLength = 128;

    /** Convert a frequency to the soundecology-style [first, last] bin range.
     *
     * soundecology computes `rows_width = n_bins / nyquist` and then indexes
     * `spec[min_freq * rows_width : max_freq * rows_width]` with R's 1-based,
     * truncating `:` operator. This reproduces that selection with 0-based bins
     * so that the firmware sums exactly the same rows as the reference.
     *
     * @param first Set to the first (0-based) bin in the band.
     * @param last Set to the last (0-based, inclusive) bin in the band.
     * @return false if the band is empty or outside the spectrum.
     */
    bool soundecologyBand(float sampleRate, float minFreq, float maxFreq, int nBins, int &first, int &last);

    /** Bioacoustic Index (Boelman et al. 2007), as implemented by soundecology::bioacoustic_index.
     *
     * For every bin in [minFreq, maxFreq] the mean power over all frames is
     * converted to dB. The index is the area between that mean spectrum and its
     * minimum within the band:
     *
     *     BI = rows_width * sum_k (L_k - min_k L_k),  L_k = 10 log10(mean_t |X_k(t)|^2)
     *
     * where rows_width = n_bins / nyquist, as in soundecology. The dB reference
     * cancels in the subtraction, so the absolute scaling of the FFT does not
     * matter.
     */
    class BioacousticIndexAccumulator
    {
    public:
        BioacousticIndexAccumulator(float sampleRate, float minFreq = 2000.0f, float maxFreq = 8000.0f);

        void reset();

        /** Add one frame of kSpectrumLength FFT magnitudes. */
        void addFrame(const float *spectrum);

        /** Bioacoustic Index for all frames since reset(), or 0 if there are none. */
        float value() const;

        long getFrameCount() const { return m_frames; }

        int getFirstBin() const { return m_first; }
        int getLastBin() const { return m_last; }

    private:
        double m_power[kSpectrumLength]; // Sum of |X_k|^2 over frames, for each bin
        long m_frames = 0;

        float m_rowsWidth; // Bins per Hz, as defined by soundecology
        int m_first;
        int m_last;
        bool m_valid;
    };

    /** Total acoustic entropy, H = Ht * Hf (Sueur et al. 2008), after seewave::H.
     *
     * Hf is the spectral entropy of the mean amplitude spectrum:
     *
     *     p_k = S_k / sum S,   Hf = -sum p_k ln p_k / ln N_bins
     *
     * Ht is the temporal entropy of the amplitude envelope. seewave uses the
     * Hilbert envelope of the raw waveform; the firmware only sees FFT frames,
     * so the envelope here is one value per frame, e(t) = sqrt(sum_k |X_k(t)|^2).
     * Ht is accumulated in streaming form using
     *
     *     -sum p ln p = ln S - (1/S) sum e ln e,  S = sum e
     *
     * so no per-frame history is stored. Both entropies are normalised to [0, 1].
     */
    class TotalEntropyAccumulator
    {
    public:
        TotalEntropyAccumulator();

        void reset();

        void addFrame(const float *spectrum);

        float spectralEntropy() const;
        float temporalEntropy() const;

        /** Total entropy H = Ht * Hf, or 0 if fewer than two frames have been added. */
        float value() const { return spectralEntropy() * temporalEntropy(); }

        long getFrameCount() const { return m_frames; }

    private:
        double m_spectrum[kSpectrumLength]; // Sum of |X_k| over frames, for each bin
        double m_envSum = 0.0;              // S = sum e(t)
        double m_envSumELogE = 0.0;         // sum e(t) ln e(t)
        long m_frames = 0;
    };

    /** Acoustic event count, after Towsey et al. (2014).
     *
     * Each frame is reduced to a band level in dB, L(t) = 10 log10(sum_{k in band} |X_k(t)|^2).
     * The background noise level is the mode of the histogram of L(t) over the
     * interval, and an event starts every time L(t) rises from below to at or
     * above (background + threshold).
     *
     * The background is only known at the end of the interval, so instead of
     * storing every frame the accumulator counts upward crossings of every
     * histogram edge as it goes. Once the mode is known, the count for the
     * edge at (mode + threshold) is the event count. This gives the same answer
     * as a two-pass count against that edge.
     */
    class AcousticEventAccumulator
    {
    public:
        static const int kLevels = 240;     // Histogram bins
        static const float kMinDb;          // Lower edge of the lowest bin (dB)
        static const float kStepDb;         // Width of a histogram bin (dB)

        AcousticEventAccumulator(float sampleRate, float minFreq = 1000.0f, float maxFreq = 8000.0f, float thresholdDb = 3.0f);

        void reset();

        void addFrame(const float *spectrum);

        /** Number of events since reset(). */
        long value() const;

        /** Background noise level (lower edge of the modal histogram bin), in dB. */
        float backgroundDb() const;

        /** Histogram bin index for a level in dB (clamped to [0, kLevels - 1]). */
        static int levelIndex(float db);

        long getFrameCount() const { return m_frames; }

    private:
        int thresholdIndex() const;

        unsigned long m_histogram[kLevels];
        unsigned long m_crossings[kLevels]; // m_crossings[i]: rises from below edge i to at/above it
        int m_previous = -1;
        long m_frames = 0;

        int m_first;
        int m_last;
        bool m_valid;
        int m_thresholdSteps;
    };

} // namespace ecoacoustics

#endif // BAIT2_ECOACOUSTICMATH_H
