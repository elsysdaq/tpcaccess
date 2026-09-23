#ifndef UTILS_TYPE_UTILS_H
#define UTILS_TYPE_UTILS_H

#include "concepts.h"

#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>

#include <cassert>
#include <cstring>

namespace utils {

/*******************************************************************************
 *  Type conversion and comparison utilities
 ******************************************************************************/

template <typename T, typename U> T enum_cast(U Val) {
    static_assert(std::is_enum<T>::value == true);
    return static_cast<T>(static_cast<std::underlying_type<T>::type>(Val));
}

template <typename U> bool bool_cast(U Val) {
    return (Val == 0) ? false : true;
}

namespace detail {
    template <std::floating_point T> bool isnan(T val) {
        return val != val;
    }
}

// Safely bounded arithmetic type conversion
template <std::integral T, std::integral U> T bounded_cast(U val) {
    constexpr T t_min = std::numeric_limits<T>::lowest();
    constexpr T t_max = std::numeric_limits<T>::max();

    // These predicates are safe for mixing signed/unsigned comparison
    if (std::cmp_greater_equal(val, t_max)) {
        return t_max;
    }
    else if (std::cmp_less_equal(val, t_min)) {
        return t_min;
    }
    return static_cast<T>(val);
}

// Safely bounded arithmetic type conversion
template <std::integral T, std::floating_point U> T bounded_cast(U val) {
    constexpr T t_min = std::numeric_limits<T>::lowest();
    constexpr T t_max = std::numeric_limits<T>::max();
    constexpr U u_min = static_cast<U>(t_min);
    constexpr U u_max = static_cast<U>(t_max);

    if (detail::isnan(val)) {
        return T{0};  // Return 0 for NaNs
    }
    else if (val >= u_max) {
        return t_max;
    }
    else if (val <= u_min) {
        return t_min;
    }
    return static_cast<T>(val);
}

// Safely bounded arithmetic type conversion
template <std::floating_point T, Arithmetic U> T bounded_cast(U val) {
    constexpr T t_min           = std::numeric_limits<T>::lowest();
    constexpr T t_max           = std::numeric_limits<T>::max();
    constexpr long double d_min = static_cast<long double>(t_min);
    constexpr long double d_max = static_cast<long double>(t_max);

    if constexpr (std::is_floating_point_v<U>) {
        if (detail::isnan(val)) {
            return static_cast<T>(val);
        }
    }
    const long double d_val = static_cast<long double>(val);

    if (d_val > d_max) {
        return t_max;
    }
    else if (d_val < d_min) {
        return t_min;
    }
    return static_cast<T>(val);
}

// Verified narrowing cast utility
template <typename T, typename U> T narrow_cast(U value) {
    auto result = static_cast<T>(value);
    assert(static_cast<U>(result) == value);  // Verify no data loss
    return result;
}

// Type conversion template that uses memcpy
// Can be used to convert between structures that have identical layouts
template <typename T, typename U> T convert(const U& in) {
    static_assert(sizeof(T) == sizeof(U), "Types must be the same size");

    static_assert(std::is_trivially_copyable_v<T>, "Source type must be trivially copyable");
    static_assert(std::is_trivially_copyable_v<U>, "Destination type must be trivially copyable");

    static_assert(std::alignment_of_v<T> <= std::alignment_of_v<U>, "Alignment requirements not met");

    T out{};
    std::memcpy(&out, &in, sizeof(T));
    return out;
}

// Equality comparison template that uses memcmp
// Warning: memcmp may return incorrect results when comparing structs with floating point values (due to signed zeros
// and NaNs)
template <typename T> int compare(const T& lhs, const T& rhs) {
    static_assert(std::is_standard_layout_v<T>, "Type must be standard layout");
    static_assert(std::has_unique_object_representations_v<T>,
                  "Type must have a unique object representation (this error is probably caused by padding)");
    static_assert(!std::is_pointer_v<T>, "Type must not be a pointer");
    return std::memcmp(&lhs, &rhs, sizeof(T));
}

/*******************************************************************************
 *  Function Type Information
 ******************************************************************************/

/**
 * Provides compile-time type information for a known function, deduced from a function pointer.
 *
 * Usage Example:
 * --------------
 * int myFunc(int a, double b, char c) { return 0; }
 *
 * using Info = FunctionTraits<myFunc>;
 *
 * // Extract return type
 * static_assert(std::is_same_v<Info::ReturnType, int>);
 *
 * // Apply parameter types to any variadic template
 * static_assert(std::is_same_v<Info::Apply<std::tuple>,   std::tuple<int, double, char>>);
 * static_assert(std::is_same_v<Info::Apply<std::variant>, std::variant<int, double, char>>);
 */

namespace detail {
    // Decomposes a function type into its return and parameter types.
    template <typename TFunc> struct FunctionSignatureTraits;

    template <typename TReturn, typename... TParams> struct FunctionSignatureTraits<TReturn(TParams...)> {
        // Instantiates TTemplate with the function's parameter types as the pack.
        template <template <typename...> class TTemplate> using Apply = TTemplate<TParams...>;

        // Deduces the return type of TCallable when invoked with the function's parameter types.
        template <typename TCallable>
        using InvokeResult = std::invoke_result_t<TCallable, decltype(std::declval<TParams>())...>;
    };
}  // namespace detail

// Provides compile-time type information for a known function, deduced from a function pointer.
template <auto TFunc> struct FunctionTraits {
   private:
    using Traits = detail::FunctionSignatureTraits<std::remove_pointer_t<decltype(TFunc)>>;

   public:
    // Instantiates TTemplate with the function's parameter types as the pack.
    template <template <typename...> class TTemplate> using Apply = typename Traits::template Apply<TTemplate>;

    // The return type of the function, deduced by invoking it with its own parameter types.
    using ReturnType = typename Traits::template InvokeResult<decltype(TFunc)>;
};

// Declares a type named "return_t" as an alias of the return type of the "func" argument
#define DEF_RETURN_T(func) using return_t = ::utils::FunctionTraits<func>::ReturnType;

}  // namespace utils

#endif  // UTILS_TYPE_UTILS_H
