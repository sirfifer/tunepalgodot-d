/**
 * Lightweight YIN Pitch Detection Algorithm
 *
 * Self-contained implementation based on:
 * "YIN, a fundamental frequency estimator for speech and music"
 * by Alain de Cheveigne and Hideki Kawahara (2002)
 *
 * This is a clean-room implementation, NOT derived from GPL code.
 * MIT License compatible.
 */

#ifndef YIN_DETECTOR_H
#define YIN_DETECTOR_H

#include <vector>
#include <cmath>

namespace tunepal_exp {

struct YinResult {
    float frequency;      // Detected frequency in Hz (-1 if no pitch found)
    float confidence;     // Confidence 0.0-1.0 (based on CMNDF minimum)
    int period_samples;   // Detected period in samples
};

class YinDetector {
public:
    // Configuration
    float threshold = 0.15f;  // YIN threshold (lower = stricter, typical: 0.10-0.20)
    float sample_rate = 44100.0f;
    float min_frequency = 80.0f;   // ~E2 (lowest reasonable for most instruments)
    float max_frequency = 2000.0f; // ~B6 (covers most melodic content)

    /**
     * Detect pitch from audio samples
     * @param samples Audio samples (mono, normalized to -1.0 to 1.0)
     * @return YinResult with frequency, confidence, and period
     */
    YinResult detect(const std::vector<float>& samples) {
        YinResult result = {-1.0f, 0.0f, 0};

        if (samples.size() < 64) {
            return result;
        }

        // Calculate tau range from frequency limits
        int tau_min = static_cast<int>(sample_rate / max_frequency);
        int tau_max = static_cast<int>(sample_rate / min_frequency);

        // Ensure we don't exceed buffer limits
        int max_tau = static_cast<int>(samples.size() / 2);
        if (tau_max > max_tau) tau_max = max_tau;
        if (tau_min < 2) tau_min = 2;
        if (tau_max <= tau_min) return result;

        // Step 1 & 2: Difference function
        std::vector<float> diff(tau_max);
        for (int tau = 0; tau < tau_max; tau++) {
            diff[tau] = 0.0f;
            for (size_t i = 0; i < samples.size() - tau_max; i++) {
                float delta = samples[i] - samples[i + tau];
                diff[tau] += delta * delta;
            }
        }

        // Step 3: Cumulative Mean Normalized Difference Function (CMNDF)
        std::vector<float> cmndf(tau_max);
        cmndf[0] = 1.0f;
        float running_sum = 0.0f;

        for (int tau = 1; tau < tau_max; tau++) {
            running_sum += diff[tau];
            if (running_sum > 0.0f) {
                cmndf[tau] = diff[tau] * tau / running_sum;
            } else {
                cmndf[tau] = 1.0f;
            }
        }

        // Step 4: Absolute threshold
        int tau_estimate = -1;
        for (int tau = tau_min; tau < tau_max; tau++) {
            if (cmndf[tau] < threshold) {
                // Find local minimum
                while (tau + 1 < tau_max && cmndf[tau + 1] < cmndf[tau]) {
                    tau++;
                }
                tau_estimate = tau;
                break;
            }
        }

        if (tau_estimate == -1) {
            return result;
        }

        // Step 5: Parabolic interpolation for sub-sample accuracy
        float better_tau = static_cast<float>(tau_estimate);
        if (tau_estimate > 0 && tau_estimate < tau_max - 1) {
            float s0 = cmndf[tau_estimate - 1];
            float s1 = cmndf[tau_estimate];
            float s2 = cmndf[tau_estimate + 1];
            float adjustment = (s2 - s0) / (2.0f * (2.0f * s1 - s2 - s0));
            if (!std::isnan(adjustment) && std::abs(adjustment) < 1.0f) {
                better_tau += adjustment;
            }
        }

        // Calculate frequency
        result.frequency = sample_rate / better_tau;
        result.period_samples = tau_estimate;

        // Confidence is inversely related to the CMNDF value at the minimum
        result.confidence = 1.0f - cmndf[tau_estimate];
        if (result.confidence < 0.0f) result.confidence = 0.0f;
        if (result.confidence > 1.0f) result.confidence = 1.0f;

        return result;
    }

    /**
     * Convert frequency to MIDI note number
     * @param frequency Frequency in Hz
     * @return MIDI note number (69 = A4 = 440Hz), or -1 if invalid
     */
    static int frequency_to_midi(float frequency) {
        if (frequency <= 0.0f) return -1;
        return static_cast<int>(std::round(69.0f + 12.0f * std::log2(frequency / 440.0f)));
    }

    /**
     * Convert frequency to note name with octave
     * @param frequency Frequency in Hz
     * @return Note name (e.g., "A4", "C#5") or empty if invalid
     */
    static const char* midi_to_note_name(int midi_note) {
        static const char* note_names[] = {
            "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
        };
        if (midi_note < 0 || midi_note > 127) return "";
        return note_names[midi_note % 12];
    }

    static int midi_to_octave(int midi_note) {
        if (midi_note < 0) return -1;
        return (midi_note / 12) - 1;
    }
};

} // namespace tunepal_exp

#endif // YIN_DETECTOR_H
