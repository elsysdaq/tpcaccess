#ifndef UTILS_MISC_H
#define UTILS_MISC_H

#include "concepts.h"
#include "type_utils.h"

#include <algorithm>
#include <array>
#include <bit>
#include <string>
#include <type_traits>
#include <utility>

#include <cstdint>

// Miscellaneous utilities that don't have a better place to go

// Macro to wrap a function name with a lambda expression. This is helpful if one would like to e.g. pass a pointer to
// an overloaded function, or a non-addressable std function, as an argument to std::transform. Types are automatically
// deduced and it is written to be noexcept/SFINAE-friendly.
#define LIFT(F)                                                                                                     \
    [](auto&&... args) noexcept(                                                                                    \
        noexcept(F(std::forward<decltype(args)>(args)...))) -> decltype(F(std::forward<decltype(args)>(args)...)) { \
        return F(std::forward<decltype(args)>(args)...);                                                            \
    }

namespace utils {

// Helper template class for iterating over an enum. Only works if enum does not have any gaps.
// Usage:
// enum foo { one, two, three };
// using fooIterator = utils::EnumIterator<foo, one, three>;
// for(auto i : fooIterator) { ... }
template <typename T, T beginVal, T endVal, typename std::enable_if_t<std::is_enum<T>::value, bool> = true>
class EnumIterator {
    using val_t = typename std::underlying_type<T>::type;
    val_t m_val;

   public:
    EnumIterator(const T& f) : m_val(static_cast<val_t>(f)) {}
    EnumIterator() : m_val(static_cast<val_t>(beginVal)) {}
    EnumIterator operator++() {
        ++m_val;
        return *this;
    }
    T operator*() { return static_cast<T>(m_val); }
    EnumIterator begin() { return *this; }
    EnumIterator end() {
        static const EnumIterator endIter = ++EnumIterator(endVal);
        return endIter;
    }
    bool operator!=(const EnumIterator& i) { return m_val != i.m_val; }
};

// Reverse order of bits from LSB to MSB-first (and vice-versa)
// Example: reverseBits(0b11001001) == 0b10010011
inline uint8_t reverseBits(uint8_t val) {
    uint64_t res = ((val * 0x80200802ULL) & 0x884422110ULL) * 0x0101010101ULL >> 32;
    return static_cast<uint8_t>(res);
}

// Shorten string to max_length and add ellipsis (...) if it has been shortened
inline std::string shorten(const std::string& str, size_t max_length) {
    if (str.length() <= max_length) {
        return str;
    }
    if (max_length < 3) {
        return str.substr(0, max_length);
    }
    return str.substr(0, max_length - 3) + "...";
}

// Force compile-time evaluation of value
// Causes compilation error if compile-time eval is not possible
consteval auto asConstant(auto value) {
    return value;
}

template <class T1, class T2> T1 endianSwap(T2 Val) {
    T1 ValT = static_cast<T1>(Val);
#ifdef TPC_BIG_ENDIAN
    uint8_t* Ptr = reinterpret_cast<uint8_t*>(&ValT);
    switch (sizeof(T1)) {
        case 2: swap(Ptr[0], Ptr[1]); break;
        case 4:
            swap(Ptr[0], Ptr[3]);
            swap(Ptr[1], Ptr[2]);
            break;
        case 8:
            swap(Ptr[0], Ptr[7]);
            swap(Ptr[1], Ptr[6]);
            swap(Ptr[2], Ptr[5]);
            swap(Ptr[3], Ptr[4]);
            break;
    }
#endif
    return ValT;
}

// WIP: Alternative endian-swap function templates

// Swap the byte order if compiled for a big-endian architecture
template <typename T> constexpr T byteswap_BE(T val) noexcept {
    static_assert(std::has_unique_object_representations_v<T>, "Type may not have padding bits");

    if constexpr (std::endian::native == std::endian::big) {
        auto val_bytes = std::bit_cast<std::array<uint8_t, sizeof(T)>>(val);
        std::reverse(val_bytes.begin(), val_bytes.end());
        return std::bit_cast<T>(val_bytes);
    }
    else {
        return val;
    }
}

// Used for persistent binary data that needs to be compatible between little-endian/big-endian architectures
template <typename T1, typename T2> constexpr T1 byteswap_cast(T2 val) noexcept {
    T1 val_cast = static_cast<T1>(val);
    return byteswap_BE(val_cast);
}

// RAII scope guard that invokes a void() callable on destruction.
// Usage: {
//          int fd = open(path.c_str(), O_WRONLY);
//          auto guard = ScopeExit([&] { close(fd); });
//          ... do stuff with fd ...
//        } // fd is automatically closed when scope exits
template <VoidCallable F> class ScopeExit {
    F m_func;
    bool m_active = true;

   public:
    ScopeExit(F fn) : m_func(std::move(fn)) {}
    ~ScopeExit() {
        if (m_active) m_func();
    }

    ScopeExit(const ScopeExit&)      = delete;
    ScopeExit& operator=(ScopeExit&) = delete;

    ScopeExit(ScopeExit&& o) noexcept : m_func(std::move(o.m_func)), m_active(std::exchange(o.m_active, false)) {}
    ScopeExit& operator=(ScopeExit&& o) noexcept {
        m_func = std::move(o.m_func);
        m_active = std::exchange(o.m_active, false);
    }

    void release() { m_active = false; }
};

}  // namespace utils

#endif  // UTILS_MISC_H
