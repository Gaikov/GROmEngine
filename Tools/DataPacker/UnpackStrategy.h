#pragma once

#include <filesystem>

class UnpackStrategy {
public:
    UnpackStrategy(const char *packedFile, const char *targetFolder, const char *propertiesFile);
    bool Perform();

private:
    std::filesystem::path _packedFile;
    std::filesystem::path _targetFolder;
    std::filesystem::path _propertiesFile;
};
