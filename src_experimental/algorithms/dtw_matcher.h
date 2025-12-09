/**
 * Dynamic Time Warping (DTW) Sequence Matcher
 *
 * Simple, self-contained DTW implementation for melody matching.
 * Optimized for comparing note sequences (not raw audio).
 *
 * MIT License compatible - clean-room implementation.
 */

#ifndef DTW_MATCHER_H
#define DTW_MATCHER_H

#include <vector>
#include <cmath>
#include <algorithm>
#include <limits>

namespace tunepal_exp {

class DtwMatcher {
public:
    // Window constraint for DTW (Sakoe-Chiba band)
    // 0 = no constraint, higher = more flexible matching
    int window_size = 50;

    /**
     * Compute DTW distance between two sequences
     * @param seq1 First sequence (e.g., recorded melody as MIDI notes)
     * @param seq2 Second sequence (e.g., database melody)
     * @return DTW distance (lower = more similar)
     */
    float distance(const std::vector<float>& seq1, const std::vector<float>& seq2) {
        if (seq1.empty() || seq2.empty()) {
            return std::numeric_limits<float>::infinity();
        }

        int n = static_cast<int>(seq1.size());
        int m = static_cast<int>(seq2.size());

        // Use 2-row optimization for memory efficiency
        std::vector<float> prev_row(m + 1, std::numeric_limits<float>::infinity());
        std::vector<float> curr_row(m + 1, std::numeric_limits<float>::infinity());

        prev_row[0] = 0.0f;

        for (int i = 1; i <= n; i++) {
            curr_row[0] = std::numeric_limits<float>::infinity();

            // Apply Sakoe-Chiba band constraint
            int j_start = 1;
            int j_end = m;
            if (window_size > 0) {
                j_start = std::max(1, i - window_size);
                j_end = std::min(m, i + window_size);
            }

            for (int j = j_start; j <= j_end; j++) {
                float cost = std::abs(seq1[i - 1] - seq2[j - 1]);

                // Take minimum of three neighbors
                float min_prev = std::min({
                    prev_row[j - 1],  // diagonal (match)
                    prev_row[j],      // vertical (insertion)
                    curr_row[j - 1]   // horizontal (deletion)
                });

                curr_row[j] = cost + min_prev;
            }

            std::swap(prev_row, curr_row);
        }

        return prev_row[m];
    }

    /**
     * Compute normalized DTW similarity (0.0 = different, 1.0 = identical)
     * @param seq1 First sequence
     * @param seq2 Second sequence
     * @return Similarity score between 0.0 and 1.0
     */
    float similarity(const std::vector<float>& seq1, const std::vector<float>& seq2) {
        float dist = distance(seq1, seq2);
        if (std::isinf(dist)) return 0.0f;

        // Normalize by path length
        int path_length = static_cast<int>(seq1.size() + seq2.size());
        float normalized = dist / static_cast<float>(path_length);

        // Convert distance to similarity (using exponential decay)
        // This maps [0, inf) -> (1, 0]
        return std::exp(-normalized / 2.0f);
    }

    /**
     * Subsequence DTW - find best match of pattern within a longer sequence
     * @param pattern Short pattern to search for
     * @param text Longer sequence to search in
     * @return Best matching score (0.0 = no match, 1.0 = perfect match)
     */
    float subsequence_match(const std::vector<float>& pattern,
                           const std::vector<float>& text) {
        if (pattern.empty() || text.empty()) return 0.0f;
        if (pattern.size() > text.size()) return 0.0f;

        int n = static_cast<int>(pattern.size());
        int m = static_cast<int>(text.size());

        // For subsequence matching, we allow starting anywhere in text
        std::vector<float> prev_row(m + 1, 0.0f);  // Initialize to 0 (can start anywhere)
        std::vector<float> curr_row(m + 1, 0.0f);

        for (int i = 1; i <= n; i++) {
            curr_row[0] = std::numeric_limits<float>::infinity();

            for (int j = 1; j <= m; j++) {
                float cost = std::abs(pattern[i - 1] - text[j - 1]);

                float min_prev = std::min({
                    prev_row[j - 1],  // diagonal
                    prev_row[j],      // vertical
                    curr_row[j - 1]   // horizontal
                });

                curr_row[j] = cost + min_prev;
            }

            std::swap(prev_row, curr_row);
        }

        // Find minimum in last row (pattern can end anywhere)
        float min_dist = *std::min_element(prev_row.begin() + 1, prev_row.end());

        // Normalize and convert to similarity
        float normalized = min_dist / static_cast<float>(n);
        return std::exp(-normalized / 2.0f);
    }

    /**
     * Convert note string (like "ABCDEFG") to MIDI sequence
     * @param notes String of note names (A-G)
     * @return Vector of MIDI-like values (for DTW comparison)
     */
    static std::vector<float> notes_to_sequence(const std::string& notes) {
        std::vector<float> seq;
        seq.reserve(notes.size());

        for (char c : notes) {
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
};

} // namespace tunepal_exp

#endif // DTW_MATCHER_H
