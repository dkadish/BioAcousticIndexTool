//
// Board-independent maths for the spectral acoustic indices. See EcoacousticMath.h.
//

#include "EcoacousticMath.h"

#include <math.h>

namespace ecoacoustics
{

    // Keeps log10 finite for silent bins/frames.
    static const double kPowerFloor = 1e-30;

    bool soundecologyBand(float sampleRate, float minFreq, float maxFreq, int nBins, int &first, int &last)
    {
        first = 0;
        last = -1;

        if (sampleRate <= 0.0f || nBins <= 0 || maxFreq < minFreq)
        {
            return false;
        }

        double rowsWidth = (double)nBins / (sampleRate / 2.0);
        double a = minFreq * rowsWidth;
        double b = maxFreq * rowsWidth;

        // R's a:b gives a, a+1, ..., a + floor(b - a); indexing truncates and is 1-based.
        int start = (int)a - 1;
        int end = (int)a + (int)floor(b - a) - 1;

        if (start < 0)
        {
            start = 0; // R silently drops index 0
        }
        if (end > nBins - 1)
        {
            end = nBins - 1;
        }
        if (end < start)
        {
            return false;
        }

        first = start;
        last = end;
        return true;
    }

    /* Bioacoustic Index */

    BioacousticIndexAccumulator::BioacousticIndexAccumulator(float sampleRate, float minFreq, float maxFreq)
    {
        m_rowsWidth = (float)kSpectrumLength / (sampleRate / 2.0f);
        m_valid = soundecologyBand(sampleRate, minFreq, maxFreq, kSpectrumLength, m_first, m_last);
        reset();
    }

    void BioacousticIndexAccumulator::reset()
    {
        for (int i = 0; i < kSpectrumLength; i++)
        {
            m_power[i] = 0.0;
        }
        m_frames = 0;
    }

    void BioacousticIndexAccumulator::addFrame(const float *spectrum)
    {
        for (int i = 0; i < kSpectrumLength; i++)
        {
            double a = spectrum[i];
            m_power[i] += a * a;
        }
        m_frames++;
    }

    float BioacousticIndexAccumulator::value() const
    {
        if (!m_valid || m_frames == 0)
        {
            return 0.0f;
        }

        double level[kSpectrumLength];
        double minLevel = 0.0;
        for (int i = m_first; i <= m_last; i++)
        {
            double meanPower = m_power[i] / (double)m_frames;
            level[i] = 10.0 * log10(meanPower > kPowerFloor ? meanPower : kPowerFloor);
            if (i == m_first || level[i] < minLevel)
            {
                minLevel = level[i];
            }
        }

        double area = 0.0;
        for (int i = m_first; i <= m_last; i++)
        {
            area += level[i] - minLevel;
        }

        return (float)(area * m_rowsWidth);
    }

    /* Total Entropy */

    TotalEntropyAccumulator::TotalEntropyAccumulator()
    {
        reset();
    }

    void TotalEntropyAccumulator::reset()
    {
        for (int i = 0; i < kSpectrumLength; i++)
        {
            m_spectrum[i] = 0.0;
        }
        m_envSum = 0.0;
        m_envSumELogE = 0.0;
        m_frames = 0;
    }

    void TotalEntropyAccumulator::addFrame(const float *spectrum)
    {
        double energy = 0.0;
        for (int i = 0; i < kSpectrumLength; i++)
        {
            double a = fabs(spectrum[i]);
            m_spectrum[i] += a;
            energy += a * a;
        }

        double e = sqrt(energy);
        m_envSum += e;
        if (e > 0.0)
        {
            m_envSumELogE += e * log(e);
        }
        m_frames++;
    }

    float TotalEntropyAccumulator::spectralEntropy() const
    {
        double total = 0.0;
        for (int i = 0; i < kSpectrumLength; i++)
        {
            total += m_spectrum[i];
        }
        if (total <= 0.0)
        {
            return 0.0f;
        }

        double h = 0.0;
        for (int i = 0; i < kSpectrumLength; i++)
        {
            double p = m_spectrum[i] / total;
            if (p > 0.0)
            {
                h -= p * log(p);
            }
        }

        return (float)(h / log((double)kSpectrumLength));
    }

    float TotalEntropyAccumulator::temporalEntropy() const
    {
        if (m_frames < 2 || m_envSum <= 0.0)
        {
            return 0.0f;
        }

        double h = log(m_envSum) - m_envSumELogE / m_envSum;
        return (float)(h / log((double)m_frames));
    }

    /* Acoustic Event Count */

    const float AcousticEventAccumulator::kMinDb = -120.0f;
    const float AcousticEventAccumulator::kStepDb = 0.5f;

    AcousticEventAccumulator::AcousticEventAccumulator(float sampleRate, float minFreq, float maxFreq, float thresholdDb)
    {
        m_valid = soundecologyBand(sampleRate, minFreq, maxFreq, kSpectrumLength, m_first, m_last);
        m_thresholdSteps = (int)floor(thresholdDb / kStepDb + 0.5f);
        reset();
    }

    void AcousticEventAccumulator::reset()
    {
        for (int i = 0; i < kLevels; i++)
        {
            m_histogram[i] = 0;
            m_crossings[i] = 0;
        }
        m_previous = -1;
        m_frames = 0;
    }

    int AcousticEventAccumulator::levelIndex(float db)
    {
        float position = (db - kMinDb) / kStepDb;
        if (!(position >= 0.0f)) // Also catches NaN
        {
            return 0;
        }
        if (position >= (float)(kLevels - 1))
        {
            return kLevels - 1;
        }
        return (int)position;
    }

    void AcousticEventAccumulator::addFrame(const float *spectrum)
    {
        if (!m_valid)
        {
            return;
        }

        double power = 0.0;
        for (int i = m_first; i <= m_last; i++)
        {
            double a = spectrum[i];
            power += a * a;
        }
        int level = levelIndex((float)(10.0 * log10(power > kPowerFloor ? power : kPowerFloor)));

        m_histogram[level]++;

        // Count every histogram edge this frame rose past. The first frame has
        // no predecessor, so a level that is already high does not count as an onset.
        if (m_previous >= 0)
        {
            for (int i = m_previous + 1; i <= level; i++)
            {
                m_crossings[i]++;
            }
        }
        m_previous = level;
        m_frames++;
    }

    int AcousticEventAccumulator::thresholdIndex() const
    {
        int mode = 0;
        for (int i = 1; i < kLevels; i++)
        {
            if (m_histogram[i] > m_histogram[mode])
            {
                mode = i;
            }
        }
        return mode + m_thresholdSteps;
    }

    long AcousticEventAccumulator::value() const
    {
        if (m_frames == 0)
        {
            return 0;
        }

        int t = thresholdIndex();
        if (t <= 0 || t >= kLevels)
        {
            return 0;
        }
        return (long)m_crossings[t];
    }

    float AcousticEventAccumulator::backgroundDb() const
    {
        return kMinDb + (thresholdIndex() - m_thresholdSteps) * kStepDb;
    }

} // namespace ecoacoustics
