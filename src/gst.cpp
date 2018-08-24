#include <algorithm>
#include <unordered_map>
#include "gst.hpp"
#include "cyclichash.h"


inline bool is_unmarked(const Token& token) noexcept {
    return not token.mark;
}


inline bool is_a_match(const Token& pattern_tok, const Token& text_tok) noexcept {
    return is_unmarked(pattern_tok) and is_unmarked(text_tok) and pattern_tok.chr == text_tok.chr;
}


inline bool all_tokens_match(const Match& match) noexcept {
    auto pattern_it = match.pattern_it;
    auto text_it = match.text_it;
    for (auto i = 0u; i < match.match_length; ++i, ++pattern_it, ++text_it) {
        if (!is_a_match(*pattern_it, *text_it)) {
            return false;
        }
    }
    return true;
}


template<class T>
inline T scanpatterns(Tokens& pattern_marks, Tokens& text_marks, Matches& matches, const T& search_length) noexcept {

    // Create rolling hashers for pattern and text substrings of length search_length,
    // with hash value type T, hash value size of 32 bits,
    // and the same random seed 1 (seed 2 is ignored in hashers with hash value sizes less than 64 bits)
    CyclicHash<T> pattern_hasher(search_length, 1u, 2u, 32u);
    CyclicHash<T> text_hasher(search_length, 1u, 2u, 32u);

    // Create a hash map of all text substring hashes,
    // enabling constant time validation of pattern and text substring mismatches
    std::unordered_map<T, std::vector<typename Tokens::iterator> > text_hash_to_positions;

    T maxmatch = 0;

    // Forward to first unmarked text position
    auto text_it_begin = std::find_if(text_marks.begin(), text_marks.end(), is_unmarked);

    if (text_it_begin + search_length > text_marks.end()) {
        // Too close to end of text, cannot create a match here
        return maxmatch;
    }

    // Compute hash value for each possible unmarked substring of search_length in text and store its starting position
    for (auto text_it = text_it_begin; text_it + search_length - 1 < text_marks.end(); ++text_it) {
        if (text_it == text_it_begin) {
            // Initialize hash using first text range
            for (auto it = text_it; it != text_it + search_length; ++it) {
                text_hasher.eat(it->chr);
            }
        } else {
            // Update rolling hash
            text_hasher.update((text_it - 1)->chr, (text_it + search_length - 1)->chr);
        }

        // Skip all strings with at least 1 marked token
        if (not std::all_of(text_it, text_it + search_length, is_unmarked)) {
            continue;
        }

        const auto& text_hash_it = text_hash_to_positions.find(text_hasher.hashvalue);

        if (text_hash_it == text_hash_to_positions.end()) {
            // Hash value does not exist, create new vector with text iterator
            std::vector<typename Tokens::iterator> v_it = { text_it };
            text_hash_to_positions.emplace(text_hasher.hashvalue, v_it);
        } else {
            // Hash value exists, add new starting point for hashed substring
            text_hash_it->second.push_back(text_it);
        }
    }

    // For each unmarked pattern character, try to find the longest matching substring

    // Forward to first unmarked pattern position
    auto pattern_it_begin = std::find_if(pattern_marks.begin(), pattern_marks.end(), is_unmarked);

    if (pattern_it_begin + search_length > pattern_marks.end()) {
        // Too close to end of pattern, cannot create a match here
        return maxmatch;
    }

    // Starting at the first unmarked pattern character, iterate over rest of the pattern
    for (auto pattern_it = pattern_it_begin; pattern_it + search_length - 1 < pattern_marks.end(); ++pattern_it) {

        if (pattern_it == pattern_it_begin) {
            // Initialize hash using first pattern range
            pattern_hasher.reset();
            for (auto it = pattern_it; it != pattern_it + search_length; ++it) {
                pattern_hasher.eat(it->chr);
            }
        } else {
            // Update rolling hash
            pattern_hasher.update((pattern_it - 1)->chr, (pattern_it + search_length - 1)->chr);
        }

        // Skip all strings with at least 1 marked token
        if (not std::all_of(pattern_it, pattern_it + search_length, is_unmarked)) {
            continue;
        }

        // Check if there is a matching text range
        const auto& text_hash_it = text_hash_to_positions.find(pattern_hasher.hashvalue);
        if (text_hash_it == text_hash_to_positions.end()) {
            // Mismatching hashes, no string match here
            continue;
        }

        // Iterate over all text positions that share the hash value of current pattern hash
        for (const auto& hash_it : text_hash_it->second) {
            // As an optimization, assume there are no hash collisions and skip
            // all characters in range [0, search_length)
            // This assumption will be validated later in markarrays
            auto pattern_jt = pattern_it + search_length;
            auto text_jt = hash_it + search_length;
            T matching_chars = search_length;

            // Count the amount of consequtive, unmarked, matching characters
            while (pattern_jt != pattern_marks.end() and text_jt != text_marks.end()
                    and is_a_match(*pattern_jt, *text_jt)) {
                ++matching_chars;
                ++pattern_jt;
                ++text_jt;
            }

            if (matching_chars > 2 * search_length) {
                // If the match is 'very long' (here an arbitrary 2 * search_length),
                // it will most likely contain many, smaller matches that are proper subsets of it.
                // Therefore, stop matching and return the long match length to restart matching
                return matching_chars;
            } else {
                // Record a match
                matches.push_back({ pattern_it, hash_it, matching_chars });
                maxmatch = std::max(maxmatch, matching_chars);
            }
        }

    }

    return maxmatch;
}


template<class T>
inline T markarrays(Tokens& pattern_marks, Tokens& text_marks, Matches& matches, Tiles& tiles) noexcept {

    T length_of_tokens_tiled = 0;

    // Iterate queue starting with the longest match
    for (auto match_it = matches.begin(); match_it != matches.end(); ++match_it) {
        const auto& maxmatch = match_it->match_length;
        if (all_tokens_match(*match_it)) {
            // All tokens of this match are unmarked, i.e. do not belong to another match.
            // Mark all the tokens of this match to prevent overlapping matches
            match_it->mark_all_tokens();
            // Get the starting indexes of this match in pattern and text
            const T& pattern_index = match_it->pattern_it - pattern_marks.begin();
            const T& text_index = match_it->text_it - text_marks.begin();
            // Create a tile to finalize this match
            tiles.push_back({ pattern_index, text_index, maxmatch });
            length_of_tokens_tiled += maxmatch;
        }
    }
    return length_of_tokens_tiled;
}


Tiles match_strings(
        const std::string& pattern,
        const std::string& text,
        const match_length_t& init_search_length,
        const std::string& init_pattern_marks,
        const std::string& init_text_marks) noexcept {

    Tiles tiles;
    if (pattern.size() < init_search_length || text.size() < init_search_length) {
        // Too short threshold for creating matches
        return tiles;
    }

    // Construct token strings with initial marks, assuming missing marks to be false

    Tokens pattern_marks;
    pattern_marks.reserve(pattern.size());
    for (auto i = 0u; i < pattern.size(); ++i) {
        const auto& init_mark = (i < init_pattern_marks.size() and init_pattern_marks[i] == '1');
        pattern_marks.push_back({ pattern[i], init_mark });
    }

    Tokens text_marks;
    text_marks.reserve(text.size());
    for (auto i = 0u; i < text.size(); ++i) {
        const auto& init_mark = (i < init_text_marks.size() and init_text_marks[i] == '1');
        text_marks.push_back({ text[i], init_mark });
    }

    match_length_t length_of_tokens_tiled = 0u;
    match_length_t search_length = init_search_length;

    // FIXME hack, for terminating loops on some pairs of input that cause infinite loops
    match_length_t prev_length_of_tokens_tiled = 1u;
    unsigned tiled_count_repeats = 0;

    Matches matches;

    // Search for all matches of maximal length and longer than search_length
    while (search_length > 0 and search_length >= init_search_length) {
        matches.clear();
        // Find all matching substrings and their lengths, and push the data to matches
        match_length_t maxmatch = scanpatterns(pattern_marks, text_marks, matches, search_length);

        if (maxmatch > 2 * search_length) {
            // Found a very long match,
            // try again with larger search_length to avoid redundant matching of subset matches
            search_length = maxmatch;
            continue;
        }

        prev_length_of_tokens_tiled = length_of_tokens_tiled;
        // Create new tiles by marking all unmarked tokens that participate in a maximal match
        length_of_tokens_tiled += markarrays<match_length_t>(pattern_marks, text_marks, matches, tiles);

        // FIXME hack, terminate loop if the amount of tokens tiled stays the same for 10 iterations
        if (length_of_tokens_tiled == prev_length_of_tokens_tiled && ++tiled_count_repeats > 10) {
            break;
        }

        if (search_length > 2 * init_search_length) {
            search_length >>= 1;
        } else if (search_length > init_search_length) {
            search_length = init_search_length;
        } else if (search_length > 0) {
            --search_length;
        }
    }

    return tiles;
}


