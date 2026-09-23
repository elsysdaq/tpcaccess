#ifndef MESSAGE_H
#define MESSAGE_H

#ifdef WIN32
#include <common/clean_windows.h>
#include <io.h>      // Windows
#else
#include <unistd.h>  // Unix
#endif

#include <common/sysThreading.h>
#include <common/utils/file_utils.h>

#include <algorithm>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace common {

bool inline isTerminal() {
#ifdef _WIN32
    return _isatty(_fileno(stdout));
#else
    return isatty(fileno(stdout));
#endif
}

class MessageStd {
   public:
    void writeLine(const char* msg) { std::cout << msg << "\n" << std::flush; }

    void write(const char* msg) { std::cout << msg; }

    void updateLine(const char* msg) {
        // Use carriage return to overwrite current line
        std::cout << "\r" << msg << std::flush;
    }

    void updatePreviousLine(const char* msg) {
        if (isTerminal()) {
            std::cout << "\033[F\033[2K" << msg << "\n";
        }
        else {
            // Fallback for file output
            std::cout << msg << "\n";
        }
        std::cout << std::flush;
    }
};

// Write log to string object
class MessageString {
   public:
    MessageString(std::string& target_string) : m_target_string(target_string) {}

    void writeLine(const char* msg) {
        m_target_string.append(msg);
        m_target_string.append("\n");
    }

    void write(const char* msg) { m_target_string.append(msg); }

    void updatePreviousLine(const char* msg) {
        if (!msg) {
            return;
        }

        if (m_target_string.empty()) {
            // Nothing to update, just write the line
            m_target_string.append(msg);
            return;
        }

        // Find the start of the last line (search backwards for last newline)
        size_t last_newline_pos  = m_target_string.find_last_of('\n');
        size_t last2_newline_pos = m_target_string.find_last_of('\n', last_newline_pos - 1);

        if (last2_newline_pos != std::string::npos) {
            // There's at least one complete line
            // Erase everything after the last newline
            m_target_string.erase(last2_newline_pos + 1);
        }
        else {
            // No newlines in the string, replace everything
            m_target_string.clear();
        }

        // Append the new message
        m_target_string.append(msg);
        m_target_string.append("\n");
    }

   private:
    std::string& m_target_string;
};

class MessageDbg {
   public:
#ifdef WIN32
    void writeLine(const char* msg) {
        OutputDebugStringA(msg);
        OutputDebugStringA("\n");
    }

    void write(const char* msg) { OutputDebugStringA(msg); }
#else
    void writeLine(const char* msg) { std::cout << msg << "\n"; }

    void write(const char* msg) { std::cout << msg; }
#endif
};

class MessageFile {
   public:
    void writeLine(const char* msg) {
        m_file << msg << "\n";
        m_file.flush();
        m_last_line_length = 0;  // Reset since we wrote newline
    }

    void write(const char* msg) {
        m_file << msg;
        m_file.flush();
    }

    // TODO replace with methods in std::filesystem
    MessageFile(const std::string& file_name, int max_log_files) {
        try {
            std::string path = "";
            std::string name;
            std::string extension;
            utils::FileUtils file;
            std::string file_name_full = file_name + ".txt";

            file.splitFileName(file_name_full, path, name, extension);

            std::vector<utils::cFileListItem> file_list;
            file.getFileList(path, name, file_list);
            std::sort(file_list.begin(), file_list.end(), utils::cFileListItem());

            for (unsigned index = max_log_files; index < file_list.size(); index++)
                file.removeFile(path + file_list[index - max_log_files].getFileName());

            unsigned file_number = 0;
            if (!file_list.empty() && max_log_files != 0) file_number = file_list.back().getFileNumber() + 1;

            std::stringstream stream;
            stream << path << name << "_" << std::setw(4) << std::setfill('0') << file_number << extension;
            this->m_file_name = stream.str();
            // move();
            m_file.open(this->m_file_name.c_str(), std::ios::out | std::ios::trunc);
        }
        catch (...) {
        }
    }

    ~MessageFile() {
        try {
            m_file.close();
        }
        catch (...) {
        }
    }

   private:
    std::fstream m_file;
    std::string m_file_name;
    size_t m_last_line_length = 0;

    std::string getFileName(bool last = false) {
        if (!last) return m_file_name + ".txt";
        return m_file_name + "_last.txt";
    }

    void move() {
        std::fstream file_in;
        std::fstream file_out;

        file_in.open(getFileName().c_str(), std::ios::in);
        if (file_in) {
            file_out.open(getFileName(true).c_str(), std::ios::out | std::ios::trunc);
            if (file_out) {
                char value;
                while (file_in.get(value)) file_out.put(value);
                file_out.close();
            }
            file_in.close();
        }
    }
};

class IMessage {
   public:
    virtual ~IMessage() {};
    virtual void debug(const char* format, ...)   = 0;
    virtual void message(const char* format, ...) = 0;
    virtual void warning(const char* format, ...) = 0;
    virtual void error(const char* format, ...)   = 0;

    virtual void debug(const char* format, va_list arg)   = 0;
    virtual void message(const char* format, va_list arg) = 0;
    virtual void warning(const char* format, va_list arg) = 0;
    virtual void error(const char* format, va_list arg)   = 0;

    virtual void showDebug(bool show = true)   = 0;
    virtual void showMessage(bool show = true) = 0;
    virtual void showWarning(bool show = true) = 0;
    virtual void showError(bool show = true)   = 0;
};

template <class MessagePolicy> class MessagePrinter : public IMessage {
   public:
    // Templated variadic constructor forwards its arguments to
    // MessagePolicy constructor
    template <class... Ts> MessagePrinter(Ts&&... args);
    // virtual ~MessagePrinter() override = default;

    void debug(const char* format, ...) override;
    void message(const char* format, ...) override;
    void warning(const char* format, ...) override;
    void error(const char* format, ...) override;

    void debug(const char* format, va_list arg) override;
    void message(const char* format, va_list arg) override;
    void warning(const char* format, va_list arg) override;
    void error(const char* format, va_list arg) override;

    virtual void showDebug(bool show = true) override { m_show_debug = show; }
    virtual void showMessage(bool show = true) override { m_show_message = show; }
    virtual void showWarning(bool show = true) override { m_show_warning = show; }
    virtual void showError(bool show = true) override { m_show_error = show; }

   private:
    bool m_show_debug;
    bool m_show_message;
    bool m_show_warning;
    bool m_show_error;
    std::string m_last_message_prefix;
    std::string m_last_message;
    unsigned m_last_message_repeated;
    std::chrono::steady_clock::time_point m_last_message_time;

    cMutex m_mutex;

    void printRepeatedMessage(std::string prefix, std::string message);
    bool hasTimeoutExpired();  // Check if 1 second has passed since last print
    std::string getTime();
    template <class T> std::string toString(T val);
    std::string toString(const char* format, va_list arg);

    MessagePolicy m_message_policy;
};
}  // namespace common

#include "message.cpp"

#endif
