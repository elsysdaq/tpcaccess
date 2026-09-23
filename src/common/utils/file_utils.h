#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <system_error>
#include <vector>

namespace utils {

class cFileListItem {
   public:
    cFileListItem() {}

    cFileListItem(const std::string& FileName) {
        m_FileName   = FileName;
        m_FileNumber = extractFileNumber();
    }

    bool operator()(const cFileListItem& I1, const cFileListItem& I2) {
        return I1.getFileNumber() < I2.getFileNumber();
    }

    const std::string& getFileName() const { return m_FileName; }
    unsigned getFileNumber() const { return m_FileNumber; }

   private:
    unsigned m_FileNumber  = 0;
    std::string m_FileName = "";
    unsigned extractFileNumber() {
        std::stringstream Stream;
        size_t Index1 = m_FileName.rfind("_");
        size_t Index2 = m_FileName.rfind(".");
        Stream << m_FileName.substr(Index1 + 1, Index2 - 1);
        Stream >> m_FileNumber;
        if (Stream.fail()) m_FileNumber = 0;
        return m_FileNumber;
    }
};

// TODO write as free functions
class FileUtils {
   public:
    void getFileList(const std::string& Path, const std::string& Name, std::vector<cFileListItem>& FileList);
    void removeFile(const std::string& FileName);
    void splitFileName(const std::string& FileName, std::string& Path, std::string& Name, std::string& Extension);
};

std::vector<char> readFileAsBinary(const std::string& filename);
std::error_code createDirIfNotExists(const std::string& directory);
std::error_code createDirIfNotExists(const std::filesystem::path& directory);

}  // namespace utils

#endif  // FILEUTILS_H
