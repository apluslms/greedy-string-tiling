#ifndef GST_H
#define GST_H
#include <string>
#include <vector>

/*
 * Tokens are single characters that can be marked.
 * Marked tokens denote characters that participate in some pair of matching substrings.
 */
struct Token {
    const char chr;
    bool mark;
};

typedef std::vector<Token> Tokens;

typedef unsigned long match_length_t;

/*
 * Matches contain starting positions of two matching substrings and the length of the match.
 * Match instances should never contain iterators that are not
 * dereferencable in the index range [0, match_length).
 */
struct Match {
    typename Tokens::iterator pattern_it;
    typename Tokens::iterator text_it;
    match_length_t match_length;

    inline void mark_all_tokens() const noexcept {
        for (auto i = 0u; i < match_length; ++i) {
            (pattern_it + i)->mark = true;
            (text_it + i)->mark = true;
        }
    };
};

typedef std::vector<Match> Matches;

/*
 * Tiles are finalized matches that should not be changed.
 */
struct Tile {
    const match_length_t pattern_index;
    const match_length_t text_index;
    const match_length_t match_length;
};

typedef std::vector<Tile> Tiles;


/*
 * For two given strings, run Karp-Rabin Greedy String tiling and return a vector of Tiles that correspond to matching substrings of maximal length from both strings.
 * Initial search length denotes the threshold of a match; substrings shorter than init_search_length are not compared.
 * Setting init_search_length to a high (low) value will considerably reduce (increase) the running time.
 * Initial marks can be given as a string of zeros and ones.
 * Marked characters cannot participate in a maximal match and substrings containing them will be skipped.
 */
Tiles match_strings(
        const std::string& pattern,
        const std::string& text,
        const match_length_t& init_search_length,
        const std::string& init_pattern_marks = "",
        const std::string& init_text_marks = "") noexcept;

#endif // GST_H
