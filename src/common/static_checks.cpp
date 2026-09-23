#include <bit>
#include <limits>
#include <type_traits>

#include <climits>
#include <cstdint>

// Some platform soundness checks, any assumptions our codebase makes about things like type sizes etc. can be put here
// to alert for any problems.

static_assert(std::is_same_v<uint8_t, unsigned char>, "uint8_t is not unsigned char on this platform");
static_assert(std::endian::native == std::endian::little, "Platform must be little-endian");
static_assert(CHAR_BIT == 8, "Platform must have 8-bit bytes");

// FP standard
static_assert(std::numeric_limits<float>::is_iec559, "float must be IEEE 754");
static_assert(std::numeric_limits<double>::is_iec559, "double must be IEEE 754");

// Check that either Linux or Win32 platform macro is available
#if !(defined(WIN32) || defined(_LINUX))
#error "Platform not supported"
#endif
