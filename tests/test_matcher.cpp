#include <functional>
#include "gst.hpp"
#include "data_generator.hpp"
#define CATCH_CONFIG_MAIN
#include "catch.hpp"


static bool all_matches_are_non_overlapping(const Tiles& tiles) {
    for (auto i = 0; i < tiles.size(); ++i) {
        for (auto j = 0; j < tiles.size(); ++j) {
            if (i == j) {
                continue;
            }
            const auto& tile_a = tiles[i];
            const auto& tile_b = tiles[j];
            if ((tile_a.pattern_index < tile_b.pattern_index
                 and tile_a.pattern_index + tile_a.match_length >= tile_b.pattern_index)
                or
                (tile_a.text_index < tile_b.text_index
                 and tile_a.text_index + tile_a.match_length >= tile_b.text_index)) {
                return false;
            }
        }
    }
    return true;
}


SCENARIO("Proper substrings of simple strings produces always at least one match when the minimum match length is half of the substring.", "[match-simple]") {
    CAPTURE(data_generator_seed);

    constexpr auto pattern_size = 4lu;
    constexpr auto init_search_length = pattern_size >> 1;
    const std::string text = "abcdefghijklmnopqrst";

    GIVEN("The pattern is a prefix of some string.") {
        const std::string pattern = text.substr(0, pattern_size);

        WHEN("Calling match_strings with the given parameters") {
            const auto tiles = match_strings(pattern, text, init_search_length);

            THEN("There is only one match, which is the prefix") {
                REQUIRE(tiles.size() == 1);
                REQUIRE(all_matches_are_non_overlapping(tiles));
                for (auto& tile : tiles) {
                    INFO(tile.pattern_index);
                    INFO(tile.text_index);
                    REQUIRE(pattern.substr(tile.pattern_index, tile.match_length)
                            == text.substr(tile.text_index, tile.match_length));
                }
            }
        }
    }

    GIVEN("The pattern is a suffix of some string.") {
        const std::string pattern = text.substr(text.size() - pattern_size, pattern_size);

        WHEN("Calling match_strings with the given parameters") {
            const auto tiles = match_strings(pattern, text, init_search_length);

            THEN("There is only one match, which is the suffix") {
                REQUIRE(tiles.size() == 1);
                REQUIRE(all_matches_are_non_overlapping(tiles));
                for (auto& tile : tiles) {
                    REQUIRE(pattern.substr(tile.pattern_index, tile.match_length)
                            == text.substr(tile.text_index, tile.match_length));
                }
            }
        }
    }

    GIVEN("The pattern is an infix of some string.") {
        const std::string pattern = text.substr((text.size() >> 1) - pattern_size, pattern_size);

        WHEN("Calling match_strings with the given parameters") {
            const auto tiles = match_strings(pattern, text, init_search_length);

            THEN("There is only one match, which is the infix") {
                REQUIRE(tiles.size() == 1);
                REQUIRE(all_matches_are_non_overlapping(tiles));
                for (auto& tile : tiles) {
                    REQUIRE(pattern.substr(tile.pattern_index, tile.match_length)
                            == text.substr(tile.text_index, tile.match_length));
                }
            }
        }
    }

}


SCENARIO("Proper substrings of simple strings produces always at least one match when the minimum match length is equal to the substring.", "[match-simple-exact]") {
    CAPTURE(data_generator_seed);

    constexpr auto pattern_size = 4lu;
    const std::string text = "abcdefghijklmnopqrst";
    constexpr auto init_search_length = pattern_size;

    GIVEN("The pattern is a prefix of some string.") {
        const std::string pattern = text.substr(0, pattern_size);

        WHEN("Calling match_strings with the given parameters") {
            const auto tiles = match_strings(pattern, text, init_search_length);

            THEN("There is only one match, which is the prefix") {
                REQUIRE(tiles.size() == 1);
                REQUIRE(all_matches_are_non_overlapping(tiles));
                for (auto& tile : tiles) {
                    REQUIRE(pattern.substr(tile.pattern_index, tile.match_length)
                            == text.substr(tile.text_index, tile.match_length));
                }
            }
        }
    }

    GIVEN("The pattern is a suffix of some string.") {
        const std::string pattern = text.substr(text.size() - pattern_size, pattern_size);

        WHEN("Calling match_strings with the given parameters") {
            const auto tiles = match_strings(pattern, text, init_search_length);

            THEN("There is only one match, which is the suffix") {
                REQUIRE(tiles.size() == 1);
                REQUIRE(all_matches_are_non_overlapping(tiles));
                for (auto& tile : tiles) {
                    REQUIRE(pattern.substr(tile.pattern_index, tile.match_length)
                            == text.substr(tile.text_index, tile.match_length));
                }
            }
        }
    }

    GIVEN("The pattern is an infix of some string.") {
        const std::string pattern = text.substr((text.size() >> 1) - pattern_size, pattern_size);

        WHEN("Calling match_strings with the given parameters") {
            const auto tiles = match_strings(pattern, text, init_search_length);

            THEN("There is only one match, which is the infix") {
                REQUIRE(tiles.size() == 1);
                REQUIRE(all_matches_are_non_overlapping(tiles));
                for (auto& tile : tiles) {
                    REQUIRE(pattern.substr(tile.pattern_index, tile.match_length)
                            == text.substr(tile.text_index, tile.match_length));
                }
            }
        }
    }

}


SCENARIO("Proper substrings of simple strings never match if all matches contain initial marks", "[nomatch-simple-marked]") {
    CAPTURE(data_generator_seed);

    constexpr auto pattern_size = 4lu;
    const std::string text = "abcdefghijklmnopqrst";
    constexpr auto init_search_length = pattern_size;

    GIVEN("The pattern is a prefix of some string, but the first character is marked.") {
        const std::string pattern = text.substr(0, pattern_size);
        std::string text_marks(text.size(), '0');
        text_marks[0] = '1';

        WHEN("Calling match_strings with the given parameters") {
            const auto tiles = match_strings(pattern, text, init_search_length, "", text_marks);

            THEN("There are no matches") {
                REQUIRE(tiles.size() == 0);
            }
        }
    }

    GIVEN("The pattern is a suffix of some string, but the first character of the suffix is marked.") {
        const std::string pattern = text.substr(text.size() - pattern_size, pattern_size);
        std::string text_marks(text.size(), '0');
        text_marks[text.size() - pattern_size] = '1';

        WHEN("Calling match_strings with the given parameters") {
            const auto tiles = match_strings(pattern, text, init_search_length, "", text_marks);

            THEN("There are no matches") {
                REQUIRE(tiles.size() == 0);
            }
        }
    }

    GIVEN("The pattern is a infix of some string, but the first character of the infix is marked.") {
        const std::string pattern = text.substr((text.size() >> 1) - pattern_size, pattern_size);
        std::string text_marks(text.size(), '0');
        text_marks[(text.size() >> 1) - pattern_size] = '1';

        WHEN("Calling match_strings with the given parameters") {
            const auto tiles = match_strings(pattern, text, init_search_length, "", text_marks);

            THEN("There are no matches") {
                REQUIRE(tiles.size() == 0);
            }
        }
    }

    GIVEN("Several matching substrings but all contain one mark.") {
        const std::string pattern = text;
        std::string text_marks(text.size(), '0');
        for (auto i = 0; i < text.size(); i += init_search_length) {
            text_marks[i] = '1';
        }

        WHEN("Calling match_strings with the given parameters") {
            const auto tiles = match_strings(pattern, text, init_search_length, "", text_marks);

            THEN("There are no matches") {
                REQUIRE(tiles.size() == 0);
            }
        }
    }

}


SCENARIO("Two matches of different length but the longer is already marked", "[match-two-marked]") {
    CAPTURE(data_generator_seed);

    constexpr auto init_search_length = 4;

    GIVEN("Matching suffix of length 4 and matching prefix of length 5, but the prefix contains a mark") {
        const std::string text =    "abcdefghijklmnopqrst";
        const std::string pattern = "abcdexxxxxxxxxxxqrst";
        std::string text_marks(text.size(), '0');
        text_marks[3] = '1'; // set mark at 'd'

        WHEN("Calling match_strings with the given parameters") {
            const auto tiles = match_strings(pattern, text, init_search_length, "", text_marks);

            THEN("There is exactly one match, which is the suffix") {
                REQUIRE(tiles.size() == 1);
                REQUIRE(all_matches_are_non_overlapping(tiles));
                const auto& tile = tiles.at(0);
                REQUIRE(text.substr(tile.pattern_index, tile.match_length) == "qrst");
            }
        }
    }

    GIVEN("Matching suffix of length 5 and matching prefix of length 4, but the suffix contains a mark") {
        const std::string text =    "abcdefghijklmnopqrst";
        const std::string pattern = "abcdxxxxxxxxxxxpqrst";
        std::string text_marks(text.size(), '0');
        text_marks[17] = '1'; // set mark at 'r'

        WHEN("Calling match_strings with the given parameters") {
            const auto tiles = match_strings(pattern, text, init_search_length, "", text_marks);

            THEN("There is exactly one match, which is the prefix") {
                REQUIRE(tiles.size() == 1);
                REQUIRE(all_matches_are_non_overlapping(tiles));
                const auto& tile = tiles.at(0);
                REQUIRE(text.substr(tile.pattern_index, tile.match_length) == "abcd");
            }
        }
    }
}


SCENARIO("Strings that have no characters in common can never have matches", "[no-match-simple]") {
    CAPTURE(data_generator_seed);

    GIVEN("Two disjoint strings") {
        const std::string text = "abcdefghijklmnopqrst";
        const std::string pattern = "uvwxyz";
        const auto init_search_length = pattern.size() >> 1;

        WHEN("Calling match_strings with the given parameters") {
            const auto tiles = match_strings(pattern, text, init_search_length);

            THEN("There are no matches") {
                REQUIRE(tiles.size() == 0);
            }
        }
    }
}


SCENARIO("Proper substrings of random strings produces always at least one match when the minimum match length is less than the substring length", "[match-random]") {
    CAPTURE(data_generator_seed);

    GIVEN("One random string of size 10000 and its substring") {
        constexpr auto text_size = 10000lu;
        const std::string text = next_string(text_size);

        constexpr auto init_search_length = 20lu;

        const auto pattern_size = next_integer(init_search_length, text_size);
        const auto pattern_begin = next_integer(0lu, text_size - pattern_size);
        const std::string pattern = text.substr(pattern_begin, pattern_size);

        WHEN("Calling match_strings with the given parameters") {
            const auto& tiles = match_strings(pattern, text, init_search_length);

            THEN("There is at least one match") {
                REQUIRE(tiles.size() > 0);
            }

            THEN("No matches overlap with others") {
                REQUIRE(all_matches_are_non_overlapping(tiles));
            }

            THEN("All matching substrings are equal in both text and pattern") {
                for (auto& tile : tiles) {
                    REQUIRE(pattern.substr(tile.pattern_index, tile.match_length)
                            == text.substr(tile.text_index, tile.match_length));
                }
            }
        }
    }
}


SCENARIO("Random strings with random marks", "[match-random-and-marks]") {
    CAPTURE(data_generator_seed);

    GIVEN("One random string of size 10000 and its substring") {
        const auto text_size = 10000lu;
        const std::string text = next_string(text_size);

        constexpr auto init_search_length = 20lu;

        const auto pattern_size = next_integer(init_search_length, text_size);
        const auto pattern_begin = next_integer(0lu, text_size - pattern_size);
        const std::string pattern = text.substr(pattern_begin, pattern_size);

        const auto is_unmarked = [](const auto& m){ return m == '0'; };

        WHEN("Both have 0.05 chance of marks") {
            const std::string pattern_marks = next_bitstring(pattern.size(), 0.05);
            const std::string text_marks = next_bitstring(text.size(), 0.05);
            const auto& tiles = match_strings(pattern, text, init_search_length, pattern_marks, text_marks);

            THEN("If there are matches, no matches overlap") {
                REQUIRE(all_matches_are_non_overlapping(tiles));
            }

            THEN("If there are matches, no matches overlap the initial marks") {
                for (auto& tile : tiles) {
                    INFO("Reported matching substrings must be identical in pattern and text");
                    REQUIRE(pattern.substr(tile.pattern_index, tile.match_length)
                            == text.substr(tile.text_index, tile.match_length));
                    INFO("Matches cannot overlap");
                    REQUIRE(std::all_of(
                                pattern_marks.begin() + tile.pattern_index,
                                pattern_marks.begin() + tile.pattern_index + tile.match_length,
                                is_unmarked));
                    REQUIRE(std::all_of(
                                text_marks.begin() + tile.text_index,
                                text_marks.begin() + tile.text_index + tile.match_length,
                                is_unmarked));
                }
            }
        }
    }
}
