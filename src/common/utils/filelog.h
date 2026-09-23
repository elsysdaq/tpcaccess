#ifndef UTILS_FILELOG_H
#define UTILS_FILELOG_H

#include <sstream>
#include <fstream>
#include <string>
#include <vector>

#include "appdata.h"

namespace utils {
// Additional logging class for producing log files seperate from main application logs
class FileLog {
   public:
    enum eEnd { end, endb };

    FileLog() {}
    FileLog(std::string log_path) : m_path(log_path) {}
    FileLog(std::string log_path, std::string log_path_aux) : m_path(log_path), m_pathAux{log_path_aux} {}

    ~FileLog() { close(); }

    // Disable copy semantics
    FileLog(FileLog& other)            = delete;
    FileLog& operator=(FileLog& other) = delete;

    template <class T>
    FileLog& operator<<(T data) {
        if (!m_enabled) return *this;

        m_stream << data << "\t";
        return *this;
    }

    template <class T>
    FileLog& operator<<(std::vector<T>& vec) {
        if (!m_enabled) return *this;

        for (unsigned idx = 0; idx < vec.size(); ++idx) m_stream << vec[idx] << "\t";

        return *this;
    }

    FileLog& operator<<(eEnd value) {
        if (!m_enabled) return *this;

        switch (value) {
            case end:
                m_logStream << m_stream.str() << "\n";
                // cout    << m_stream.str() << endl;
                break;
            case endb:
                m_logStream << m_stream.str() << "\n";
                if (m_enabledAux) m_logStreamAux << m_stream.str() << "\n";
                // cout     << m_stream.str() << endl;
                break;
        }
        clearStream();
        return *this;
    }

    template <typename... Ts>
    static std::string strcat(Ts... args) {
        std::ostringstream s;
        try {
            (s << ... << args);
        }
        catch (...) {
        }
        return s.str();
    }

    void enable(bool use_auxiliary_log = false) {
        m_enabled    = true;
        m_enabledAux = use_auxiliary_log;
        open();
    }

    void disable() {
        close();
        m_enabled    = false;
        m_enabledAux = false;
    }

    void setFilePath(std::string path, std::string path_aux = "") {
        close();
        m_path = path;
        if (path_aux != "") {
            m_pathAux = path_aux;
        }
        clearStream();
        open();
    }

   private:
    bool m_enabled    = false;
    bool m_enabledAux = false;
    bool m_isOpen     = false;
    bool m_isOpenAux  = false;

    std::string m_path    = getApplicationDataPath() + "log.txt";
    std::string m_pathAux = getApplicationDataPath() + "logAux.txt";

    std::stringstream m_stream;
    std::ofstream m_logStream;
    std::ofstream m_logStreamAux;

    void clearStream() {
        m_stream.str("");
        m_stream.clear();
        m_stream.setf(std::ios::left | std::ios_base::fixed);
    }

    void open() {
        if (m_enabled && !m_isOpen) {
            m_logStream.open(m_path);
            m_isOpen = true;
        }
        if (m_enabledAux && !m_isOpenAux) {
            m_logStreamAux.open(m_pathAux);
            m_isOpenAux = true;
        }
    }

    void close() {
        if (m_isOpen) {
            m_logStream.close();
            m_isOpen = false;
        }
        if (m_isOpenAux) {
            m_logStreamAux.close();
            m_isOpenAux = false;
        }
    }
};
}  // namespace utils

#endif  // UTILS_FILELOG_H
