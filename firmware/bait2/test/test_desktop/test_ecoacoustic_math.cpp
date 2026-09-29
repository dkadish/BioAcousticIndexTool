// Desktop unit tests for the board-independent index maths.
//
// Run with: pio test -e desktop

#include <unity.h>
#include <math.h>
#include <stdlib.h>

#include "EcoacousticMath.h"

using namespace ecoacoustics;

static const float kSampleRate = 44100.0f;
static const int N = kSpectrumLength;

static float frame[kSpectrumLength];

static void fill(float value)
{
    for (int i = 0; i < N; i++)
    {
        frame[i] = value;
    }
}

static float amplitudeForDb(float db)
{
    return powf(10.0f, db / 20.0f);
}

void setUp(void) {}

void tearDown(void) {}

/* Band selection */

void test_band_matches_soundecology_rows(void)
{
    // rows_width = 128 / 22050; 2000:8000 Hz -> R rows 11.6:46.4 -> 1-based 11..45 -> 0-based 10..44
    int first, last;
    TEST_ASSERT_TRUE(soundecologyBand(kSampleRate, 2000.0f, 8000.0f, N, first, last));
    TEST_ASSERT_EQUAL_INT(10, first);
    TEST_ASSERT_EQUAL_INT(44, last);
}

void test_band_clamps_to_spectrum(void)
{
    int first, last;
    TEST_ASSERT_TRUE(soundecologyBand(kSampleRate, 0.0f, 30000.0f, N, first, last));
    TEST_ASSERT_EQUAL_INT(0, first);
    TEST_ASSERT_EQUAL_INT(N - 1, last);
}

void test_band_rejects_inverted_range(void)
{
    int first, last;
    TEST_ASSERT_FALSE(soundecologyBand(kSampleRate, 8000.0f, 2000.0f, N, first, last));
}

/* Bioacoustic Index */

void test_bi_is_zero_without_frames(void)
{
    BioacousticIndexAccumulator bi(kSampleRate);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, bi.value());
}

void test_bi_is_zero_for_flat_spectrum(void)
{
    BioacousticIndexAccumulator bi(kSampleRate);
    fill(0.01f);
    for (int t = 0; t < 10; t++)
    {
        bi.addFrame(frame);
    }
    TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.0f, bi.value());
}

void test_bi_is_area_above_band_minimum(void)
{
    BioacousticIndexAccumulator bi(kSampleRate);
    int first = bi.getFirstBin(), last = bi.getLastBin();

    fill(1.0f);
    frame[first + 3] = 0.1f; // 20 dB below the rest of the band
    frame[0] = 0.0001f;      // Outside the band, must be ignored
    bi.addFrame(frame);

    float rowsWidth = N / (kSampleRate / 2.0f);
    float expected = (last - first) * 20.0f * rowsWidth;
    TEST_ASSERT_FLOAT_WITHIN(1e-4f, expected, bi.value());
}

void test_bi_averages_power_not_amplitude(void)
{
    // Mean power of 1 and 0 is 0.5 -> -3.01 dB relative to the rest of the band.
    BioacousticIndexAccumulator bi(kSampleRate);
    int first = bi.getFirstBin(), last = bi.getLastBin();

    fill(1.0f);
    bi.addFrame(frame);
    fill(1.0f);
    frame[first] = 0.0f;
    bi.addFrame(frame);

    float rowsWidth = N / (kSampleRate / 2.0f);
    float expected = (last - first) * 10.0f * log10f(2.0f) * rowsWidth;
    TEST_ASSERT_FLOAT_WITHIN(1e-4f, expected, bi.value());
}

void test_bi_ignores_overall_gain(void)
{
    BioacousticIndexAccumulator quiet(kSampleRate), loud(kSampleRate);
    srand(1);
    for (int t = 0; t < 50; t++)
    {
        for (int i = 0; i < N; i++)
        {
            frame[i] = (float)rand() / RAND_MAX;
        }
        quiet.addFrame(frame);
        for (int i = 0; i < N; i++)
        {
            frame[i] *= 1000.0f;
        }
        loud.addFrame(frame);
    }
    TEST_ASSERT_FLOAT_WITHIN(1e-3f, quiet.value(), loud.value());
}

void test_bi_reset_clears_frames(void)
{
    BioacousticIndexAccumulator bi(kSampleRate);
    fill(1.0f);
    frame[bi.getFirstBin()] = 0.1f;
    bi.addFrame(frame);
    bi.reset();
    TEST_ASSERT_EQUAL(0, bi.getFrameCount());
    TEST_ASSERT_EQUAL_FLOAT(0.0f, bi.value());
}

/* Total Entropy */

void test_hf_is_one_for_flat_spectrum(void)
{
    TotalEntropyAccumulator h;
    fill(0.5f);
    h.addFrame(frame);
    TEST_ASSERT_FLOAT_WITHIN(1e-6f, 1.0f, h.spectralEntropy());
}

void test_hf_is_zero_for_pure_tone(void)
{
    TotalEntropyAccumulator h;
    fill(0.0f);
    frame[20] = 1.0f;
    h.addFrame(frame);
    TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.0f, h.spectralEntropy());
}

void test_ht_is_one_for_constant_envelope(void)
{
    TotalEntropyAccumulator h;
    fill(0.25f);
    for (int t = 0; t < 100; t++)
    {
        h.addFrame(frame);
    }
    TEST_ASSERT_FLOAT_WITHIN(1e-6f, 1.0f, h.temporalEntropy());
    TEST_ASSERT_FLOAT_WITHIN(1e-6f, 1.0f, h.value());
}

void test_ht_is_zero_for_single_click(void)
{
    TotalEntropyAccumulator h;
    for (int t = 0; t < 100; t++)
    {
        fill(t == 50 ? 1.0f : 0.0f);
        h.addFrame(frame);
    }
    TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.0f, h.temporalEntropy());
}

void test_streaming_ht_matches_two_pass(void)
{
    const int T = 500;
    double envelope[T];
    TotalEntropyAccumulator h;

    srand(2);
    for (int t = 0; t < T; t++)
    {
        double energy = 0.0;
        for (int i = 0; i < N; i++)
        {
            frame[i] = (float)rand() / RAND_MAX * (t % 7 + 1);
            energy += (double)frame[i] * frame[i];
        }
        envelope[t] = sqrt(energy);
        h.addFrame(frame);
    }

    double sum = 0.0;
    for (int t = 0; t < T; t++)
    {
        sum += envelope[t];
    }
    double expected = 0.0;
    for (int t = 0; t < T; t++)
    {
        double p = envelope[t] / sum;
        expected -= p * log(p);
    }
    expected /= log((double)T);

    TEST_ASSERT_FLOAT_WITHIN(1e-5f, (float)expected, h.temporalEntropy());
}

void test_entropy_is_zero_without_frames(void)
{
    TotalEntropyAccumulator h;
    TEST_ASSERT_EQUAL_FLOAT(0.0f, h.value());
}

/* Acoustic Event Count */

// Frame whose band level is `db` (all energy in one in-band bin).
static void levelFrame(AcousticEventAccumulator &events, float db)
{
    fill(0.0f);
    frame[30] = amplitudeForDb(db);
    events.addFrame(frame);
}

void test_level_index(void)
{
    TEST_ASSERT_EQUAL_INT(0, AcousticEventAccumulator::levelIndex(-500.0f));
    TEST_ASSERT_EQUAL_INT(AcousticEventAccumulator::kLevels - 1, AcousticEventAccumulator::levelIndex(50.0f));
    TEST_ASSERT_EQUAL_INT(120, AcousticEventAccumulator::levelIndex(-59.75f));
}

void test_events_counts_bursts_above_background(void)
{
    AcousticEventAccumulator events(kSampleRate);
    for (int burst = 0; burst < 4; burst++)
    {
        for (int t = 0; t < 20; t++)
        {
            levelFrame(events, -59.75f);
        }
        for (int t = 0; t < 3; t++)
        {
            levelFrame(events, -49.75f);
        }
    }
    levelFrame(events, -59.75f);

    TEST_ASSERT_EQUAL(4, events.value());
    TEST_ASSERT_FLOAT_WITHIN(1e-4f, -60.0f, events.backgroundDb());
}

void test_events_ignores_rises_below_threshold(void)
{
    AcousticEventAccumulator events(kSampleRate, 1000.0f, 8000.0f, 3.0f);
    for (int burst = 0; burst < 5; burst++)
    {
        for (int t = 0; t < 10; t++)
        {
            levelFrame(events, -59.75f);
        }
        levelFrame(events, -57.25f); // +2.5 dB: below threshold
        levelFrame(events, -56.75f); // +3.0 dB: reaches threshold, one event
    }
    TEST_ASSERT_EQUAL(5, events.value());
}

void test_events_does_not_count_initial_high_level(void)
{
    AcousticEventAccumulator events(kSampleRate);
    levelFrame(events, -40.0f);
    for (int t = 0; t < 20; t++)
    {
        levelFrame(events, -59.75f);
    }
    TEST_ASSERT_EQUAL(0, events.value());
}

void test_events_matches_two_pass_count(void)
{
    const int T = 2000;
    int levels[T];
    AcousticEventAccumulator events(kSampleRate);

    srand(3);
    for (int t = 0; t < T; t++)
    {
        // Mostly background with occasional loud frames
        float db = -70.0f + (float)(rand() % 8) * 0.5f + ((rand() % 10) == 0 ? (float)(rand() % 30) : 0.0f) + 0.25f;
        levels[t] = AcousticEventAccumulator::levelIndex(db);
        levelFrame(events, db);
    }

    int histogram[AcousticEventAccumulator::kLevels] = {0};
    for (int t = 0; t < T; t++)
    {
        histogram[levels[t]]++;
    }
    int mode = 0;
    for (int i = 1; i < AcousticEventAccumulator::kLevels; i++)
    {
        if (histogram[i] > histogram[mode])
        {
            mode = i;
        }
    }
    int threshold = mode + 6; // 3 dB in 0.5 dB steps
    long expected = 0;
    for (int t = 1; t < T; t++)
    {
        if (levels[t - 1] < threshold && levels[t] >= threshold)
        {
            expected++;
        }
    }

    TEST_ASSERT_TRUE(expected > 10);
    TEST_ASSERT_EQUAL(expected, events.value());
}

void test_events_is_zero_without_frames(void)
{
    AcousticEventAccumulator events(kSampleRate);
    TEST_ASSERT_EQUAL(0, events.value());
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_band_matches_soundecology_rows);
    RUN_TEST(test_band_clamps_to_spectrum);
    RUN_TEST(test_band_rejects_inverted_range);

    RUN_TEST(test_bi_is_zero_without_frames);
    RUN_TEST(test_bi_is_zero_for_flat_spectrum);
    RUN_TEST(test_bi_is_area_above_band_minimum);
    RUN_TEST(test_bi_averages_power_not_amplitude);
    RUN_TEST(test_bi_ignores_overall_gain);
    RUN_TEST(test_bi_reset_clears_frames);

    RUN_TEST(test_hf_is_one_for_flat_spectrum);
    RUN_TEST(test_hf_is_zero_for_pure_tone);
    RUN_TEST(test_ht_is_one_for_constant_envelope);
    RUN_TEST(test_ht_is_zero_for_single_click);
    RUN_TEST(test_streaming_ht_matches_two_pass);
    RUN_TEST(test_entropy_is_zero_without_frames);

    RUN_TEST(test_level_index);
    RUN_TEST(test_events_counts_bursts_above_background);
    RUN_TEST(test_events_ignores_rises_below_threshold);
    RUN_TEST(test_events_does_not_count_initial_high_level);
    RUN_TEST(test_events_matches_two_pass_count);
    RUN_TEST(test_events_is_zero_without_frames);

    return UNITY_END();
}
