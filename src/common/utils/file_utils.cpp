#include "file_utils.h"

#ifdef WIN32
#include <common/clean_windows.h>
#else
#include <dirent.h>
#endif

#include <filesystem>
#include <system_error>

#include <stdint.h>

namespace utils {

namespace fs = std::filesystem;
using namespace std;

#ifdef WIN32
void FileUtils::getFileList(const string& Path, const string& Name, vector<cFileListItem>& FileList) {
    WIN32_FIND_DATAA FindFileData;
    HANDLE Handle = FindFirstFileA((Path + Name + "*").c_str(), &FindFileData);
    if (Handle != INVALID_HANDLE_VALUE) {
        FileList.push_back(cFileListItem(FindFileData.cFileName));
        while (FindNextFileA(Handle, &FindFileData) != 0) FileList.push_back(cFileListItem(FindFileData.cFileName));
        FindClose(Handle);
    }
}

void FileUtils::removeFile(const string& FileName) {
    DeleteFileA(FileName.c_str());
}
#else
void FileUtils::getFileList(const string& Path, const string& Name, vector<cFileListItem>& FileList) {
    DIR* Dir = opendir(Path.c_str());
    if (Dir != 0) {
        struct dirent* Dirent;
        while ((Dirent = readdir(Dir)) != 0) {
            string FileName = Dirent->d_name;
            size_t Pos      = FileName.find(Name);
            if (Pos == 0 && Pos != string::npos) FileList.push_back(cFileListItem(FileName));
        }
        closedir(Dir);
    }
}

void FileUtils::removeFile(const string& FileName) {
    ::remove(FileName.c_str());
}
#endif

void FileUtils::splitFileName(const string& FileName, string& Path, string& Name, string& Extension) {
    Path         = "";
    Name         = FileName;
    Extension    = ".bdf";
    size_t Index = FileName.rfind(".");
    if (Index != string::npos) {
        Name      = FileName.substr(0, Index);
        Extension = FileName.substr(Index, FileName.size());
    }
#ifdef WIN32
    Index = FileName.rfind("\\");
    if (Index == string::npos) {
        Index = FileName.rfind(":");
        if (Index != string::npos) Path = FileName.substr(0, Index + 1);
    }
    else
        Path = FileName.substr(0, Index + 1);

    Name = Name.substr(Path.size(), string::npos);
#else
    Index = FileName.rfind("/");
    if (Index != string::npos) {
        Path = FileName.substr(0, Index + 1);
        Name = Name.substr(Path.size(), string::npos);
    }
    else
        Path = "./";
#endif
}

std::vector<char> readFileAsBinary(const std::string& filename) {
    std::ifstream file(filename.c_str(), std::ios::binary);

    if (file) {
        file.unsetf(std::ios::skipws);
        std::streampos fileSize;

        file.seekg(0, std::ios::end);
        fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        // reserve capacity
        std::vector<char> vec(fileSize, 0);

        // read the data:
        file.read(&vec.front(), fileSize);
        return vec;
    }
    else {
        throw std::runtime_error("Error reading file: " + filename);
    }
}

std::error_code createDirIfNotExists(const std::string& directory) {
    fs::path dir{directory};
    return createDirIfNotExists(dir);
}

std::error_code createDirIfNotExists(const fs::path& directory) {
    std::error_code err{};
    if (!fs::exists(directory)) {
        fs::create_directory(directory, err);
    }
    return err;
}

}  // namespace utils
