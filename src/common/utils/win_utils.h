#ifndef UTILS_WIN_UTILS_H
#define UTILS_WIN_UTILS_H

// Windows platform-specific non-portable utilities

#ifndef WIN32
#error "win_utils.h is only available on Windows platforms. Do not include this header on non-Windows builds."
#else

#include <common/clean_windows.h>

#include <string>

namespace utils {

// Internal implementation for wide to UTF-8 conversion
namespace detail {
    inline std::string wideToUtf8Impl(const wchar_t* wide, int length) {
        if (wide == nullptr) return std::string();

        int size = WideCharToMultiByte(CP_UTF8, 0, wide, length, nullptr, 0, nullptr, nullptr);
        if (size == 0) return std::string();

        // Adjust size for null terminator if length was -1
        int resultSize = (length == -1) ? size - 1 : size;
        std::string result(resultSize, 0);
        WideCharToMultiByte(CP_UTF8, 0, wide, length, &result[0], size, nullptr, nullptr);

        return result;
    }

    inline std::wstring utf8ToWideImpl(const char* utf8, int length) {
        if (utf8 == nullptr) return std::wstring();

        int size = MultiByteToWideChar(CP_UTF8, 0, utf8, length, nullptr, 0);
        if (size == 0) return std::wstring();

        // Adjust size for null terminator if length was -1
        int resultSize = (length == -1) ? size - 1 : size;
        std::wstring result(resultSize, 0);
        MultiByteToWideChar(CP_UTF8, 0, utf8, length, &result[0], size);

        return result;
    }
}  // namespace detail

// Convert wide string to UTF-8
inline std::string wideToUtf8(const std::wstring& wide) {
    if (wide.empty()) return std::string();
    return detail::wideToUtf8Impl(wide.c_str(), -1);
}

// Convert wide string to UTF-8 (raw array overload)
inline std::string wideToUtf8(const wchar_t* wide, size_t length) {
    if (length == 0) return std::string();
    return detail::wideToUtf8Impl(wide, static_cast<int>(length));
}

// Convert wide string to UTF-8 (unchecked null-terminated version)
[[deprecated("Use wideToUtf8(const wchar_t*, size_t) for safer bounds-checked conversion")]]
inline std::string wideToUtf8(const wchar_t* wide) {
    return detail::wideToUtf8Impl(wide, -1);
}

// Convert UTF-8 to wide string
inline std::wstring utf8ToWide(const std::string& utf8) {
    if (utf8.empty()) return std::wstring();
    return detail::utf8ToWideImpl(utf8.c_str(), -1);
}

// Convert UTF-8 to wide string (raw array overload)
inline std::wstring utf8ToWide(const char* utf8, size_t length) {
    if (length == 0) return std::wstring();
    return detail::utf8ToWideImpl(utf8, static_cast<int>(length));
}

// Convert UTF-8 to wide string (unchecked null-terminated version)
[[deprecated("Use utf8ToWide(const char*, size_t) for safer bounds-checked conversion")]]
inline std::wstring utf8ToWide(const char* utf8) {
    return detail::utf8ToWideImpl(utf8, -1);
}
}  // namespace utils

#endif  // WIN32
#endif  // UTILS_WIN_UTILS_H
