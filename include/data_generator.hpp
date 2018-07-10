#ifndef DATA_GENERATOR_HPP
#define DATA_GENERATOR_HPP
#include <random>

// Each translation unit will have its own PRNG state
static std::random_device rd;
static const auto data_generator_seed = rd();
static std::default_random_engine e(data_generator_seed);

char next_ascii_char() {
    static std::uniform_int_distribution<char> random_printable_ascii(33, 126);
    return random_printable_ascii(e);
}

template<class T>
T next_integer(const T& min_val, const T& max_val) {
    std::uniform_int_distribution<T> random_integer(min_val, max_val);
    return random_integer(e);
}

template<class T>
std::string next_string(T text_size) {
    std::string text;
    while(text_size-- > 0) {
        text += next_ascii_char();
    }
    return text;
}

template<class T>
std::string next_bitstring(T size, float p) {
    std::bernoulli_distribution bernoulli(p);
    std::string s;
    while(size-- > 0) {
        s += bernoulli(e) ? '1' : '0';
    }
    return s;
}

std::string random_string_copy(const std::string& src, float copy_prob) {
    std::bernoulli_distribution bernoulli(copy_prob);
    std::string dest;
    for (const auto& c : src) {
        dest += bernoulli(e) ? c : next_ascii_char();
    }
    return dest;
}

#endif // DATA_GENERATOR_HPP
