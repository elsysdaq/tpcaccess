#ifndef LIBTPCHW_MESSAGE_TPP
#define LIBTPCHW_MESSAGE_TPP

#include "message.h"

#include <common/sysThreading.h>

#include <sstream>
#include <type_traits>

#include <cstdarg>
#include <cstdio>
#include <ctime>

namespace common {

template <class MessagePolicy>
template <class... Ts>
MessagePrinter<MessagePolicy>::MessagePrinter(Ts&&... args) : m_message_policy(std::forward<Ts>(args)...) {
    m_show_debug   = true;
    m_show_message = true;
    m_show_warning = true;
    m_show_error   = true;

    m_last_message          = "";
    m_last_message_repeated = 0;
    m_last_message_time     = std::chrono::steady_clock::now();
}

// Template specialization of constructor when MessagePolicy parameter is MessageFile 
// Turns off debug messages for release builds that are writing to a log file
template <>
template <class... Ts>
MessagePrinter<MessageFile>::MessagePrinter(Ts&&... args) : m_message_policy(std::forward<Ts>(args)...) {
    m_show_debug   = false;
    m_show_message = true;
    m_show_warning = true;
    m_show_error   = true;

    m_last_message          = "";
    m_last_message_repeated = 0;
    m_last_message_time     = std::chrono::steady_clock::now();
}

template <class MessagePolicy> void MessagePrinter<MessagePolicy>::debug(const char* format, ...) {
    va_list arg;
    va_start(arg, format);
    debug(format, arg);
    va_end(arg);
}

template <class MessagePolicy> void MessagePrinter<MessagePolicy>::message(const char* format, ...) {
    va_list arg;
    va_start(arg, format);
    message(format, arg);
    va_end(arg);
}

template <class MessagePolicy> void MessagePrinter<MessagePolicy>::warning(const char* format, ...) {
    va_list arg;
    va_start(arg, format);
    warning(format, arg);
    va_end(arg);
}

template <class MessagePolicy> void MessagePrinter<MessagePolicy>::error(const char* format, ...) {
    va_list arg;
    va_start(arg, format);
    error(format, arg);
    va_end(arg);
}

template <class MessagePolicy> void MessagePrinter<MessagePolicy>::debug(const char* format, va_list arg) {
    if (!m_show_debug) return;
    cScopedLock lock(m_mutex);
    printRepeatedMessage(getTime() + " debug : ", toString(format, arg));
}

template <class MessagePolicy> void MessagePrinter<MessagePolicy>::message(const char* format, va_list arg) {
    if (!m_show_message) return;
    cScopedLock lock(m_mutex);
    printRepeatedMessage(std::string(), toString(format, arg));
}

template <class MessagePolicy> void MessagePrinter<MessagePolicy>::warning(const char* format, va_list arg) {
    if (!m_show_warning) return;
    cScopedLock lock(m_mutex);
    printRepeatedMessage(getTime() + " warning : ", toString(format, arg));
}

template <class MessagePolicy> void MessagePrinter<MessagePolicy>::error(const char* format, va_list arg) {
    if (!m_show_error) return;
    cScopedLock lock(m_mutex);
    printRepeatedMessage(getTime() + " error : ", toString(format, arg));
}

template <class MessagePolicy> bool MessagePrinter<MessagePolicy>::hasTimeoutExpired() {
    auto current_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - m_last_message_time).count();
    return duration >= 5000;  // 1 second timeout
}

template <typename MessagePolicy>
concept HasUpdatePreviousLine = requires(MessagePolicy t, const char* msg) {
    { t.updatePreviousLine(msg) } -> std::same_as<void>;
};

template <class MessagePolicy>
void MessagePrinter<MessagePolicy>::printRepeatedMessage(std::string prefix, std::string message) {
    bool is_new_message  = (m_last_message != message);
    bool timeout_expired = hasTimeoutExpired();
    if (!is_new_message && !timeout_expired) {
        ++m_last_message_repeated;
        if constexpr (HasUpdatePreviousLine<MessagePolicy>) {
            // Update current line in-place
            std::string full_message = m_last_message_prefix + message + " [Repeated " + toString(m_last_message_repeated) + "x]";
            m_message_policy.updatePreviousLine(full_message.c_str());
        }
        // Skip if message policy does not support updating the previous message
    }
    else {
        if constexpr (!HasUpdatePreviousLine<MessagePolicy>) {
            // Print repeat message in a seperate line
            if (m_last_message_repeated > 1) {
                std::string repeat_message = " [Previous message repeated " + toString(m_last_message_repeated) + "x]";
                m_message_policy.writeLine(repeat_message.c_str());
            }
        }
        // Print the new message
        std::string full_message = prefix + message;
        m_message_policy.writeLine(full_message.c_str());

        // Reset state for new message
        m_last_message_prefix   = prefix;
        m_last_message          = message;
        m_last_message_repeated = 1;
        m_last_message_time     = std::chrono::steady_clock::now();
    }
}

// TODO use safer C++ standard library function for this
template <class MessagePolicy> std::string MessagePrinter<MessagePolicy>::getTime() {
    time_t time_value;
    time(&time_value);
    tm* local_time;
#ifdef _MSC_VER
    tm local_time_obj{};
    local_time = &local_time_obj;
    localtime_s(local_time, &time_value);
#else
    local_time = localtime(&time_value);
#endif
    std::stringstream str;
    str << local_time->tm_mday << "." << local_time->tm_mon + 1 << "." << local_time->tm_year + 1900 << " ";
    str.fill('0');
    str.width(2);
    str << local_time->tm_hour << ":";
    str.width(2);
    str << local_time->tm_min << ":";
    str.width(2);
    str << local_time->tm_sec;

    return str.str();
}

template <class MessagePolicy> template <class T> std::string MessagePrinter<MessagePolicy>::toString(T val) {
    std::stringstream str;
    try {
        str << val;
    }
    catch (...) {
    }
    return str.str();
}

template <class MessagePolicy> std::string MessagePrinter<MessagePolicy>::toString(const char* format, va_list arg) {
    char buffer[1024];
#ifdef _MSC_VER
    _vsnprintf_s(buffer, 1024, format, arg);
#else
    vsnprintf(buffer, 1024, format, arg);
#endif

    return buffer;
}
}  // namespace common

#endif  // LIBTPCHW_MESSAGE_TPP
