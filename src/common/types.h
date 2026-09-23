#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <TpcAccess.h>

#include "utils/expected.h"

#include <optional>

// This header defines generic and commonly used types/type-aliases that are shared between projects
// These are put in the global namespace for convenience, but take care with naming conflicts

// Template alias for std::expected that uses TPC_ErrorCode as an error type
// Usage example in function declaration: TpcExpect<int> intReturningFunctionWithErrors();
template <typename T>
using TpcExpect = tl::expected<T, TPC_ErrorCode>;

// For use with std::visit
// See https://www.cppstories.com/2018/09/visit-variants/
template<class... Ts>
struct Overloads : Ts... { using Ts::operator()...; };

// Type of printf and our logging functions
using printf_t = void (*)(const char*, ...);

#endif  // COMMON_TYPES_H
