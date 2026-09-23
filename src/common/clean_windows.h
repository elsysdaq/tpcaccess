#ifndef COMMON_CLEAN_WINDOWS_H
#define COMMON_CLEAN_WINDOWS_H

// Use this header instead of including "windows.h" directly
// Reduces compilation size and prevents name clashes from "min" and "max" macros
// A few windows headers like "gdiplus.h" may break with NOMINMAX
// Needs to be included above other Win32 headers that include "windows.h" or "windef.h" themselves

#ifndef WIN32
#error "clean_windows.h is only available on Windows platforms. Do not include this header on non-Windows builds."
#else

// Makes "windows.h" include fewer things
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#define CLEAN_WIN_WIN32_LEAN_AND_MEAN_DEFINED
#endif

// Prevents "min" and "max" macros being defined
#ifndef NOMINMAX
#define NOMINMAX
#define CLEAN_WIN_NOMINMAX_DEFINED
#endif

#include <windows.h>

// Undefine the macros if they weren't set elsewhere
#ifdef CLEAN_WIN_WIN32_LEAN_AND_MEAN_DEFINED
#undef WIN32_LEAN_AND_MEAN
#undef CLEAN_WIN_WIN32_LEAN_AND_MEAN_DEFINED
#endif

#ifdef CLEAN_WIN_NOMINMAX_DEFINED
#undef NOMINMAX
#undef CLEAN_WIN_NOMINMAX_DEFINED
#endif

#endif  // WIN32

#endif  // COMMON_CLEAN_WINDOWS_H
