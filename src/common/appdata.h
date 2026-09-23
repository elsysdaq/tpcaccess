#ifndef APPDATA_H
#define APPDATA_H

#include <common/logging.h>
#include <common/utils/file_utils.h>

#include <filesystem>
#include <stdexcept>
#include <string>

#ifdef WIN32
#include "clean_windows.h"
#include <shlobj.h>

static const char* MfgPathOld = "\\Elsys AG";
static const char* MfgPath    = "\\Elsys";
static const char* AppName    = "\\TpcServer";
#endif

#ifdef WIN32
inline std::string getAppdataPath() {
    char path[MAX_PATH];
    if (SHGetFolderPathA(nullptr, CSIDL_COMMON_APPDATA, nullptr, 0, path) != S_OK) {
        throw std::runtime_error("Failed to get AppData path");
    }
    return {path};
}

inline std::string getUserDocumentPath() {
    char path[MAX_PATH];
    if (SHGetFolderPathA(nullptr, CSIDL_COMMON_DESKTOPDIRECTORY, nullptr, 0, path) != S_OK) {
        throw std::runtime_error("Failed to get AppData path");
    }
    return {path};
}
inline void moveSettingsToNewPath() {
    std::string AppPath = getAppdataPath();
    std::string From    = AppPath + MfgPathOld + AppName;
    std::string To      = AppPath + MfgPath + AppName;

    MoveFileA((From + "\\TpcServer.ini").c_str(), (To + "\\TpcServer.ini").c_str());
    MoveFileA((From + "\\tpcxhw.ini").c_str(), (To + "\\tpcxhw.ini").c_str());
}
#endif

inline std::string getApplicationDataPath() {
#ifdef WIN32
    std::string AppPath = getAppdataPath();
    AppPath += MfgPath;
    CreateDirectoryA(AppPath.c_str(), 0);
    AppPath += AppName;
    bool Success = CreateDirectoryA(AppPath.c_str(), 0) != 0;
    if (Success) moveSettingsToNewPath();

    std::string LogPath = AppPath + "\\log";
    CreateDirectoryA(LogPath.c_str(), 0);

    AppPath += "\\";
    return AppPath;
#elif _LINUX
    return "";
#endif
}

inline std::string getUserDocumentsPath() {
#ifdef WIN32
    namespace fs      = std::filesystem;
    fs::path doc_path = getUserDocumentPath();
    doc_path /= "Autosequence Data";

    std::error_code err = utils::createDirIfNotExists(doc_path);
    if (err) {
        error("Error creating Autosequence Data directory, error=%d (%s)", err.value(), err.message().c_str());
    }

    return doc_path.string();
#elif _LINUX
    return "";
#endif
}

inline std::string getHiddenExtIODisablePath() {
#ifdef WIN32
    return "";
#elif _LINUX
    return "/elsys/prog/";
#endif
}

#endif
