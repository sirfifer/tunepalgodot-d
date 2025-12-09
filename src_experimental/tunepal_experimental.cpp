/**
 * TunepalExperimental - Implementation
 *
 * Experimental detection algorithms (pYIN + DTW) for A/B testing.
 * Does NOT modify Bryan's original Tunepal class.
 */

#include "tunepal_experimental.h"
#include "algorithms/yin_detector.h"
#include "algorithms/dtw_matcher.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

// Internal algorithm instances
static tunepal_exp::YinDetector yin_detector;
static tunepal_exp::DtwMatcher dtw_matcher;

void TunepalExperimental::_bind_methods() {
    // Basic test methods
    ClassDB::bind_method(D_METHOD("say_hello"), &TunepalExperimental::say_hello);
    ClassDB::bind_method(D_METHOD("get_version"), &TunepalExperimental::get_version);

    // Pitch detection
    ClassDB::bind_method(D_METHOD("detect_pitch_pyin", "audio_buffer"),
                         &TunepalExperimental::detect_pitch_pyin);
    ClassDB::bind_method(D_METHOD("detect_pitch_sequence", "audio_data", "hop_size"),
                         &TunepalExperimental::detect_pitch_sequence);
    ClassDB::bind_method(D_METHOD("detect_pitch_ensemble", "audio_buffer"),
                         &TunepalExperimental::detect_pitch_ensemble);

    // Configuration
    ClassDB::bind_method(D_METHOD("set_pitch_config", "sample_rate", "buffer_size",
                                  "min_freq", "max_freq"),
                         &TunepalExperimental::set_pitch_config);
    ClassDB::bind_method(D_METHOD("set_pitch_algorithm", "algorithm"),
                         &TunepalExperimental::set_pitch_algorithm);

    // DTW matching
    ClassDB::bind_method(D_METHOD("dtw_distance", "seq1", "seq2"),
                         &TunepalExperimental::dtw_distance);
    ClassDB::bind_method(D_METHOD("dtw_similarity", "pattern", "text"),
                         &TunepalExperimental::dtw_similarity);
    ClassDB::bind_method(D_METHOD("dtw_search", "pattern", "candidates", "max_results"),
                         &TunepalExperimental::dtw_search);

    // Needleman-Wunsch (for comparison with Bryan's algorithm)
    ClassDB::bind_method(D_METHOD("needleman_wunsch", "pattern", "text"),
                         &TunepalExperimental::needleman_wunsch);

    // Debug/tuning methods
    ClassDB::bind_method(D_METHOD("get_last_confidence"),
                         &TunepalExperimental::get_last_confidence);
    ClassDB::bind_method(D_METHOD("set_yin_threshold", "threshold"),
                         &TunepalExperimental::set_yin_threshold);
    ClassDB::bind_method(D_METHOD("get_yin_threshold"),
                         &TunepalExperimental::get_yin_threshold);
}

TunepalExperimental::TunepalExperimental() {
    // Initialize with default config
    pitch_config_.sample_rate = 44100.0f;
    pitch_config_.buffer_size = 2048;
    pitch_config_.min_frequency = 80.0f;
    pitch_config_.max_frequency = 2000.0f;
    pitch_config_.algorithm = PitchConfig::PYIN;
    last_confidence_ = 0.0f;
}

TunepalExperimental::~TunepalExperimental() {
}

// ========================================
// Basic test methods
// ========================================

void TunepalExperimental::say_hello() {
    UtilityFunctions::print("Hello from TunepalExperimental! (pYIN + DTW algorithms)");
}

String TunepalExperimental::get_version() {
    return "0.1.0-experimental";
}

// ========================================
// Pitch Detection
// ========================================

float TunepalExperimental::detect_pitch_pyin(const PackedFloat32Array& audio_buffer) {
    if (audio_buffer.size() < 64) {
        last_confidence_ = 0.0f;
        return -1.0f;
    }

    // Copy to std::vector for our algorithm
    std::vector<float> samples(audio_buffer.size());
    for (int i = 0; i < audio_buffer.size(); i++) {
        samples[i] = audio_buffer[i];
    }

    // Configure detector
    yin_detector.sample_rate = pitch_config_.sample_rate;
    yin_detector.min_frequency = pitch_config_.min_frequency;
    yin_detector.max_frequency = pitch_config_.max_frequency;

    // Detect pitch
    auto result = yin_detector.detect(samples);
    last_confidence_ = result.confidence;

    return result.frequency;
}

Array TunepalExperimental::detect_pitch_sequence(const PackedFloat32Array& audio_data,
                                                  int hop_size) {
    Array pitches;

    if (audio_data.size() < pitch_config_.buffer_size) {
        return pitches;
    }

    // Configure detector
    yin_detector.sample_rate = pitch_config_.sample_rate;
    yin_detector.min_frequency = pitch_config_.min_frequency;
    yin_detector.max_frequency = pitch_config_.max_frequency;

    int frame_size = pitch_config_.buffer_size;
    int num_frames = (audio_data.size() - frame_size) / hop_size + 1;

    for (int frame = 0; frame < num_frames; frame++) {
        int start = frame * hop_size;

        // Extract frame
        std::vector<float> samples(frame_size);
        for (int i = 0; i < frame_size; i++) {
            samples[i] = audio_data[start + i];
        }

        // Detect pitch
        auto result = yin_detector.detect(samples);

        // Create result dictionary
        Dictionary frame_result;
        frame_result["frequency"] = result.frequency;
        frame_result["confidence"] = result.confidence;
        frame_result["midi"] = frequency_to_midi(result.frequency);

        pitches.append(frame_result);
    }

    return pitches;
}

float TunepalExperimental::detect_pitch_ensemble(const PackedFloat32Array& audio_buffer) {
    // For now, just use YIN. Ensemble would combine multiple algorithms.
    // TODO: Add MPM, HPS for voting
    return detect_pitch_pyin(audio_buffer);
}

void TunepalExperimental::set_pitch_config(float sample_rate, int buffer_size,
                                           float min_freq, float max_freq) {
    pitch_config_.sample_rate = sample_rate;
    pitch_config_.buffer_size = buffer_size;
    pitch_config_.min_frequency = min_freq;
    pitch_config_.max_frequency = max_freq;
}

void TunepalExperimental::set_pitch_algorithm(int algorithm) {
    pitch_config_.algorithm = static_cast<PitchConfig::Algorithm>(algorithm);
}

// ========================================
// DTW Matching
// ========================================

float TunepalExperimental::dtw_distance(const PackedFloat32Array& seq1,
                                         const PackedFloat32Array& seq2) {
    std::vector<float> s1(seq1.size());
    std::vector<float> s2(seq2.size());

    for (int i = 0; i < seq1.size(); i++) s1[i] = seq1[i];
    for (int i = 0; i < seq2.size(); i++) s2[i] = seq2[i];

    return dtw_matcher.distance(s1, s2);
}

float TunepalExperimental::dtw_similarity(const String& pattern, const String& text) {
    // Convert note strings to sequences
    std::vector<float> p = string_to_pitch_sequence(pattern);
    std::vector<float> t = string_to_pitch_sequence(text);

    if (p.empty() || t.empty()) return 0.0f;

    // Use subsequence matching for melody search
    return dtw_matcher.subsequence_match(p, t);
}

Array TunepalExperimental::dtw_search(const String& pattern, const Array& candidates,
                                       int max_results) {
    Array results;

    std::vector<float> pattern_seq = string_to_pitch_sequence(pattern);
    if (pattern_seq.empty()) return results;

    // Score all candidates
    std::vector<std::pair<float, int>> scores;
    for (int i = 0; i < candidates.size(); i++) {
        String candidate = candidates[i];
        std::vector<float> candidate_seq = string_to_pitch_sequence(candidate);

        if (!candidate_seq.empty()) {
            float sim = dtw_matcher.subsequence_match(pattern_seq, candidate_seq);
            scores.push_back({sim, i});
        }
    }

    // Sort by similarity (descending)
    std::sort(scores.begin(), scores.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });

    // Return top results
    int count = std::min(max_results, static_cast<int>(scores.size()));
    for (int i = 0; i < count; i++) {
        Dictionary result;
        result["index"] = scores[i].second;
        result["similarity"] = scores[i].first;
        results.append(result);
    }

    return results;
}

// ========================================
// Needleman-Wunsch (for comparison)
// ========================================

float TunepalExperimental::needleman_wunsch(const String& pattern, const String& text) {
    // Simple edit distance implementation
    // This is similar to Bryan's edSubstring but standalone

    int m = pattern.length();
    int n = text.length();

    if (m == 0) return static_cast<float>(n);
    if (n == 0) return static_cast<float>(m);

    // Use 2-row optimization
    std::vector<int> prev_row(n + 1);
    std::vector<int> curr_row(n + 1);

    // Initialize - for substring matching, start cost is 0
    for (int j = 0; j <= n; j++) prev_row[j] = 0;

    for (int i = 1; i <= m; i++) {
        curr_row[0] = i;  // Deletion cost

        for (int j = 1; j <= n; j++) {
            int cost = (pattern[i - 1] == text[j - 1]) ? 0 : 1;

            curr_row[j] = std::min({
                prev_row[j] + 1,      // Deletion
                curr_row[j - 1] + 1,  // Insertion
                prev_row[j - 1] + cost // Substitution
            });
        }

        std::swap(prev_row, curr_row);
    }

    // Find minimum in last row (substring matching)
    int min_dist = *std::min_element(prev_row.begin(), prev_row.end());

    return static_cast<float>(min_dist);
}

// ========================================
// Debug/Tuning methods
// ========================================

float TunepalExperimental::get_last_confidence() {
    return last_confidence_;
}

void TunepalExperimental::set_yin_threshold(float threshold) {
    yin_detector.threshold = threshold;
}

float TunepalExperimental::get_yin_threshold() {
    return yin_detector.threshold;
}

// ========================================
// Internal helpers
// ========================================

int TunepalExperimental::frequency_to_midi(float frequency) {
    if (frequency <= 0.0f) return -1;
    return static_cast<int>(std::round(69.0f + 12.0f * std::log2f(frequency / 440.0f)));
}

float TunepalExperimental::midi_to_frequency(int midi_note) {
    if (midi_note < 0 || midi_note > 127) return -1.0f;
    return 440.0f * std::pow(2.0f, (midi_note - 69) / 12.0f);
}

std::vector<float> TunepalExperimental::string_to_pitch_sequence(const String& note_string) {
    std::vector<float> seq;
    seq.reserve(note_string.length());

    for (int i = 0; i < note_string.length(); i++) {
        char32_t c = note_string[i];
        float value = -1.0f;

        switch (c) {
            case 'C': case 'c': value = 0.0f; break;
            case 'D': case 'd': value = 2.0f; break;
            case 'E': case 'e': value = 4.0f; break;
            case 'F': case 'f': value = 5.0f; break;
            case 'G': case 'g': value = 7.0f; break;
            case 'A': case 'a': value = 9.0f; break;
            case 'B': case 'b': value = 11.0f; break;
        }

        if (value >= 0.0f) {
            seq.push_back(value);
        }
    }

    return seq;
}
