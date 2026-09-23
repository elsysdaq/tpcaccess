#ifndef LOGGING_H
#define LOGGING_H

#include "types.h"

#include <memory>

#ifdef WIN32_DISABLED
#define TPCXHW_EXP __declspec(dllexport)
#else
#define TPCXHW_EXP
#endif

// Globally namespaced logging functions
// Use identically as with printf

// Enhancement ideas:
// - Optional version of these functions with a prefix
// - Linux inspired "dynamic debug" logging
// - Ability to "chain together" multiple logger policies

TPCXHW_EXP void debug(const char* format, ...);
TPCXHW_EXP void message(const char* format, ...);
TPCXHW_EXP void warning(const char* format, ...);
TPCXHW_EXP void error(const char* format, ...);

namespace common {
extern const char* kLogFileName;
class IMessage;

void setLogger(std::unique_ptr<common::IMessage> new_logger);
void swapLogger(std::unique_ptr<common::IMessage>& new_logger);

void hexdump(void* pAddressIn, long lSize, printf_t print_func = message);

}  // namespace common

#endif
