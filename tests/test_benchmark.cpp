#include <cassert>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>

#include "gst.hpp"
#include "data_generator.hpp"


struct Result {
    double min_time = std::numeric_limits<double>::max();
    double max_time = 0;
    double total_time = 0;
    match_length_t iterations = 0;
    match_length_t match_count = 0;
    float string_similarity = 0;
    match_length_t str_len = 0;
};

template<class T>
struct TestArgs {
    const T iterations;
    const T text_size;
    const T pattern_size;
    const float random_copy_prob;
    explicit TestArgs(T a, T b, T c, float d) :
        iterations(a),
        text_size(b),
        pattern_size(c),
        random_copy_prob(d) {}
};

constexpr auto table_width = 15;

void dump_result_header(std::ostream& os) {
    os << std::setw(table_width - 5) << "iterations"
       << std::setw(table_width) << "string length"
       << std::setw(table_width) << "similarity Pr"
       << std::setw(table_width) << "min (s)"
       << std::setw(table_width) << "max (s)"
       << std::setw(table_width) << "avg (s)"
       << std::setw(table_width) << "total (s)"
       << std::setw(table_width) << "total matches"
       << std::endl;
}

std::ostream& operator<<(std::ostream& os, const Result& res) {
    auto avg = res.total_time / res.iterations;
    os << std::setprecision(4)
       << std::setw(table_width - 5) << res.iterations
       << std::setw(table_width) << res.str_len
       << std::setw(table_width) << res.string_similarity
       << std::setw(table_width) << res.min_time
       << std::setw(table_width) << res.max_time
       << std::setw(table_width) << avg
       << std::setw(table_width) << res.total_time
       << std::setw(table_width) << res.match_count;
    return os;
}

template<class T>
Result bench_match_strings(const TestArgs<T>& args) {
    const auto init_search_length = std::min(20lu, args.pattern_size);
    auto iterations = args.iterations;
    Result res;

    while(iterations-- > 0) {
        const std::string text = next_string(args.text_size);
        const std::string pattern = random_string_copy(text, args.random_copy_prob);

        auto start = std::chrono::high_resolution_clock::now();
        const auto& tiles = match_strings(pattern, text, init_search_length);
        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> elapsed = end - start;
        auto elapsed_c = elapsed.count();
        res.max_time = std::max(res.max_time, elapsed_c);
        res.min_time = std::min(res.min_time, elapsed_c);
        res.total_time += elapsed_c;
        res.iterations++;
        res.string_similarity = args.random_copy_prob;
        res.str_len = args.text_size;

        for (auto& tile : tiles) {
            const auto& pattern_str = pattern.substr(tile.pattern_index, tile.match_length);
            const auto& text_str = text.substr(tile.text_index, tile.match_length);
            if (pattern_str != text_str) {
                std::cerr << "FALSE MATCH\n" << pattern_str << "\n!=\n" << text_str << std::endl;
                assert(false);
            }
        }
        res.match_count += tiles.size();
    }

    return res;
}

int main() {

    std::cout << "\nBENCHMARKING\n" << std::endl;
    std::cout << "Tiny random strings" << std::endl;
    {
        double total_time = 0;
        constexpr auto iterations = 500;
        constexpr auto text_len = 20;
        constexpr auto pattern_len = text_len;
        dump_result_header(std::cout);
        for (auto p = 0; p <= 4; ++p) {
            const float copy_prob = 0.5f + p / 8.0f;
            const TestArgs<match_length_t> args{ iterations, text_len, pattern_len, copy_prob};
            auto res = bench_match_strings(args);
            std::cout << res  << std::endl;
            total_time += res.total_time;
        }
        std::cout << "5 * " << iterations << " iterations, total (sec): " << std::setprecision(2) << total_time << std::endl;
        std::cout << std::endl;
    }

    std::cout << "Short random strings" << std::endl;
    {
        double total_time = 0;
        constexpr auto iterations = 250;
        constexpr auto text_len = 1000;
        constexpr auto pattern_len = text_len;
        dump_result_header(std::cout);
        for (auto p = 0; p <= 4; ++p) {
            const float copy_prob = 0.5f + p / 8.0f;
            const TestArgs<match_length_t> args{ iterations, text_len, pattern_len, copy_prob};
            auto res = bench_match_strings(args);
            std::cout << res  << std::endl;
            total_time += res.total_time;
        }
        std::cout << "5 * " << iterations << " iterations, total (sec): " << std::setprecision(2) << total_time << std::endl;
        std::cout << std::endl;
    }

    std::cout << "Long random strings" << std::endl;
    {
        double total_time = 0;
        constexpr auto iterations = 25;
        constexpr auto text_len = 50000;
        constexpr auto pattern_len = text_len;
        dump_result_header(std::cout);
        for (auto p = 0; p <= 4; ++p) {
            const float copy_prob = 0.5f + p / 8.0f;
            const TestArgs<match_length_t> args{ iterations, text_len, pattern_len, copy_prob};
            auto res = bench_match_strings(args);
            std::cout << res  << std::endl;
            total_time += res.total_time;
        }
        std::cout << "5 * " << iterations << " iterations, total (sec): " << std::setprecision(2) << total_time << std::endl;
        std::cout << std::endl;
    }

    std::cout << "Very long random strings" << std::endl;
    {
        double total_time = 0;
        constexpr auto iterations = 5;
        constexpr auto text_len = 200000;
        constexpr auto pattern_len = text_len;
        dump_result_header(std::cout);
        for (auto p = 0; p <= 4; ++p) {
            const float copy_prob = 0.5f + p / 8.0f;
            const TestArgs<match_length_t> args{ iterations, text_len, pattern_len, copy_prob};
            auto res = bench_match_strings(args);
            std::cout << res  << std::endl;
            total_time += res.total_time;
        }
        std::cout << "5 * " << iterations << " iterations, total (sec): " << std::setprecision(2) << total_time << std::endl;
        std::cout << std::endl;
    }

    std::cout << "Excessively long random strings" << std::endl;
    {
        double total_time = 0;
        constexpr auto iterations = 1;
        constexpr auto text_len = 1000000;
        constexpr auto pattern_len = text_len;
        dump_result_header(std::cout);
        for (auto p = 0; p <= 4; ++p) {
            const float copy_prob = 0.5f + p / 8.0f;
            const TestArgs<match_length_t> args{ iterations, text_len, pattern_len, copy_prob};
            auto res = bench_match_strings(args);
            std::cout << res  << std::endl;
            total_time += res.total_time;
        }
        std::cout << "5 * " << iterations << " iterations, total (sec): " << std::setprecision(2) << total_time << std::endl;
        std::cout << std::endl;
    }
}
