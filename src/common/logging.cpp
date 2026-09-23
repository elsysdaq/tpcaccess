#include "logging.h"

#include "appdata.h"
#include "message.h"
#include "types.h"

#include <utility>

namespace common {

// Static singleton pattern
static std::unique_ptr<IMessage>& getLogger() {
    static std::unique_ptr<IMessage> logger;
    if (!logger) {
        logger = std::make_unique<MessagePrinter<MessageStd>>();
    }
    return logger;
}

void setLogger(std::unique_ptr<IMessage> new_logger) {
    getLogger() = std::move(new_logger);
}

void swapLogger(std::unique_ptr<IMessage>& new_logger) {
    getLogger().swap(new_logger);
}

void hexdump(void* pAddressIn, long lSize, printf_t print_func) {
    char szBuf[100];
    long lIndent = 1;
    long lOutLen, lIndex, lIndex2, lOutLen2;
    long lRelPos;
    struct {
        char* pData;
        unsigned long lSize;
    } buf;
    unsigned char *pTmp, ucTmp;
    unsigned char* pAddress = (unsigned char*)pAddressIn;

    buf.pData = (char*)pAddress;
    buf.lSize = lSize;

    while (buf.lSize > 0) {
        pTmp    = (unsigned char*)buf.pData;
        lOutLen = (int)buf.lSize;
        if (lOutLen > 16) lOutLen = 16;

        // create a 64-character formatted output line:
        snprintf(szBuf, sizeof(szBuf),
                 " >                            "
                 "                      "
                 "    %08tx",
                 (std::ptrdiff_t)(pTmp - pAddress));
        lOutLen2 = lOutLen;

        for (lIndex = 1 + lIndent, lIndex2 = 53 - 15 + lIndent, lRelPos = 0; lOutLen2;
             lOutLen2--, lIndex += 2, lIndex2++) {
            ucTmp = *pTmp++;

            snprintf(szBuf + lIndex, sizeof(szBuf) - lIndex, "%02X ", (unsigned short)ucTmp);
            if (!isprint(ucTmp)) ucTmp = '.';  // nonprintable char
            szBuf[lIndex2] = ucTmp;

            if (!(++lRelPos & 3))  // extra blank after 4 bytes
            {
                lIndex++;
                szBuf[lIndex + 2] = ' ';
            }
        }

        if (!(lRelPos & 3)) lIndex--;

        szBuf[lIndex]     = '<';
        szBuf[lIndex + 1] = ' ';

        print_func("%s", szBuf);

        buf.pData += lOutLen;
        buf.lSize -= lOutLen;
    }
}

}  // namespace common

using common::getLogger;

void debug(const char* format, ...) {
    va_list arg;
    va_start(arg, format);
    getLogger()->debug(format, arg);
    va_end(arg);
}

void message(const char* format, ...) {
    va_list arg;
    va_start(arg, format);
    getLogger()->message(format, arg);
    va_end(arg);
}

void warning(const char* format, ...) {
    va_list arg;
    va_start(arg, format);
    getLogger()->warning(format, arg);
    va_end(arg);
}

void error(const char* format, ...) {
    va_list arg;
    va_start(arg, format);
    getLogger()->error(format, arg);
    va_end(arg);
}
