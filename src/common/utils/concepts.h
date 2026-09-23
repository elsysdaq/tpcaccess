#ifndef UTILS_CONCEPTS_H
#define UTILS_CONCEPTS_H

#include <concepts>
#include <type_traits>

// Generically useful concept definitions

namespace utils {

// int, long, float, double, etc..
template <typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

// A callable type with a void(void) signature
template <typename F>
concept VoidCallable = std::invocable<F> && std::is_void_v<std::invoke_result_t<F>>;

template <typename F, typename... Args>
concept VoidInvocable = std::invocable<F, Args...> && std::is_void_v<std::invoke_result_t<F, Args...>>;

}

#endif  //  UTILS_CONCEPTS_H
