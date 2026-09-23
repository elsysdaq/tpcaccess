#ifndef UTILS_TPCMATH_H
#define UTILS_TPCMATH_H

#include "concepts.h"

#include <stdexcept>
#include <type_traits>
#include <vector>

#include <cmath>
#include <stdint.h>

// Collection of utility mathematical functions

namespace utils {

// Round up to next highest power of two
template <Arithmetic T> T pow2ceil(T x) {
    if (x <= 0) return 0;
    // Imprecise results are possible
    return static_cast<T>(std::pow(2, std::ceil(std::log2(x))));
}

template <> inline uint32_t pow2ceil(uint32_t x) {
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return ++x;
}

template <> inline int pow2ceil(int x) {
    if (x <= 0) {
        return 0;
    }
    return static_cast<int>(pow2ceil(static_cast<uint32_t>(x)));
}

// Round down to the next lower power of two
template <Arithmetic T> T pow2floor(T x) {
    return pow2ceil(x) / T{2};
}

// Returns vector containing powers of two in the interval [a, b]
// Only implemented for positive a and b
template <Arithmetic T> std::vector<T> pow2array(T a, T b) {
    // Ensure that 0 < a <= b
    if (a <= 0 || a > b) return {};

    std::vector<T> res;
    for (T val = pow2ceil(a); val <= b; val *= 2) {
        res.push_back(val);
    }
    return res;
}

// Returns the number of bits in input that are ones
inline int popCount(uint32_t x) {
    // MSVC manages to auto-vectorize this so we use it instead of popcnt primitives
    x = x - ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x + (x >> 4)) & 0x0f0f0f0f;
    x = x + (x >> 8);
    x = x + (x >> 16);
    return x & 0x0000003f;
}

inline int popCount(uint16_t x) {
    x = x - ((x >> 1) & 0x5555);
    x = (x & 0x3333) + ((x >> 2) & 0x3333);
    x = (x + (x >> 4)) & 0x0f0f;
    x = x + (x >> 8);
    return x & 0x0001f;
}

inline int popCount(uint8_t x) {
    x = x - ((x >> 1) & 0x55);
    x = (x & 0x33) + ((x >> 2) & 0x33);
    x = (x + (x >> 4));
    return x & 0x0f;
}

// Round x up to the nearest multiple of some value
// e.g. roundCeilMultiple(102, 20) == 120
template <std::integral T> T roundCeilMult(T x, T mult) {
    if (mult == 0) return x;

    T rem = x % mult;
    if (rem == 0) {
        return x; 
    }
    return (x - rem) + mult;
}

// T and U should be collections of unsigned fixed width integer types
template <typename T, typename U> std::vector<int> bitwiseCorrelation(const T& seq1, const U& seq2) {
    auto s1 = seq1.size();
    auto s2 = seq2.size();
    // This lets us assume that seq1 is the larger sequence
    if (s1 - s2 < 0) return bitwiseCorrelation(seq2, seq1);

    std::vector<int> res(s1 - s2 + 1);  // If both arrays are the same size then their "convolution" has 1 element

    for (size_t i = 0; i <= s1 - s2; ++i) {
        size_t accum = 0;
        for (size_t j = 0; j < s2; ++j) {
            typename T::value_type val1 = seq1[j + i];
            typename T::value_type val2 = seq2[j];
            accum += popCount(static_cast<typename T::value_type>(val1 ^ val2));
            // accum += std::bitset<sizeof(T::value_type) * 8>{ (T::value_type)(val1 ^ val2) }.count();
        }
        res[i] = static_cast<int>(accum);
    }
    return res;
}

template <typename T, typename U> std::vector<int> bitwiseCorrelationSummed(const T& seq1, const U& seq2) {
    auto s1 = seq1.size();
    auto s2 = seq2.size();
    // This lets us assume that seq1 is the larger sequence
    if (s1 - s2 < 0) throw new std::logic_error("Size of seq2 needs to be greater than size of seq1");

    std::vector<int> res(s2);

    for (size_t i = 0; i <= s1 - s2; ++i) {
        int accum = 0;
        for (size_t j = 0; j < s2; ++j) {
            typename T::value_type val1 = seq1[j + i];
            typename T::value_type val2 = seq2[j];
            accum += popCount(static_cast<typename T::value_type>(val1 ^ val2));
        }
        res[i % s2] += static_cast<int>(accum);
    }
    return res;
}
}  // namespace utils

#endif  // UTILS_TPCMATH_H
