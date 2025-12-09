/**
 * TunepalExperimental - Experimental Detection Algorithms
 *
 * This is a PARALLEL implementation that does NOT modify Bryan's original
 * Tunepal class. Both libraries can be loaded simultaneously for A/B testing.
 *
 * MIT-Licensed Sources:
 *   - sevagh/pitch-detection: https://github.com/sevagh/pitch-detection
 *   - asardaes/dtw-cpp: https://github.com/asardaes/dtw-cpp
 *
 * NOT derived from GPL-licensed code (FolkFriend, original pYIN, etc.)
 */

#ifndef TUNEPAL_EXPERIMENTAL_H
#define TUNEPAL_EXPERIMENTAL_H

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/string.hpp>
#include <vector>
#include <map>

namespace godot {

// Configuration for pitch detection
struct PitchConfig {
    float sample_rate = 44100.0f;
    int buffer_size = 2048;
    float min_frequency = 80.0f;   // ~E2
    float max_frequency = 2000.0f; // ~B6
    enum Algorithm { PYIN, MPM, YIN, ENSEMBLE } algorithm = PYIN;
};

class TunepalExperimental : public Node2D {
    GDCLASS(TunepalExperimental, Node2D)

private:
    PitchConfig pitch_config_;
    float last_confidence_ = 0.0f;

protected:
    static void _bind_methods();

public:
    TunepalExperimental();
    ~TunepalExperimental();

    // ========================================
    // Basic test method
    // ========================================
    void say_hello();
    String get_version();

    // ========================================
    // Pitch Detection (to be implemented with pYIN)
    // ========================================
    float detect_pitch_pyin(const PackedFloat32Array& audio_buffer);
    Array detect_pitch_sequence(const PackedFloat32Array& audio_data, int hop_size);
    float detect_pitch_ensemble(const PackedFloat32Array& audio_buffer);

    // Configuration
    void set_pitch_config(float sample_rate, int buffer_size,
                          float min_freq, float max_freq);
    void set_pitch_algorithm(int algorithm);  // 0=PYIN, 1=MPM, 2=YIN, 3=ENSEMBLE

    // ========================================
    // Sequence Matching (DTW primary)
    // ========================================
    float dtw_distance(const PackedFloat32Array& seq1, const PackedFloat32Array& seq2);
    float dtw_similarity(const String& pattern, const String& text);
    Array dtw_search(const String& pattern, const Array& candidates, int max_results);

    // ========================================
    // Needleman-Wunsch (fallback, for comparison)
    // ========================================
    float needleman_wunsch(const String& pattern, const String& text);

    // ========================================
    // Debug/Tuning methods (for nerd knobs UI)
    // ========================================
    float get_last_confidence();
    void set_yin_threshold(float threshold);
    float get_yin_threshold();

private:
    // Internal helpers
    int frequency_to_midi(float frequency);
    float midi_to_frequency(int midi_note);
    std::vector<float> string_to_pitch_sequence(const String& note_string);
};

} // namespace godot

#endif // TUNEPAL_EXPERIMENTAL_H
