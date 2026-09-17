#pragma once

#include <filesystem>
#include <vector>

#include "Args.h"
#include "Core/AssetCrypto.h"
#include "Core/PackArch.h"

class nsPackStrategy {
public:
    explicit nsPackStrategy(const Args &args);
    bool Perform();

private:
    struct sourceFile_t {
        std::filesystem::path absolutePath;
        std::string archivePath;
        packFileDesc_t desc = {};
    };

    const Args &_args;
    nsAssetCrypto::Key _key = {};
    std::vector<sourceFile_t> _files;

    bool Enumerate(const std::filesystem::path &sourceFolder);
    bool WriteArchive(const std::filesystem::path &outputFile);
    static bool ShouldExclude(const std::filesystem::path &relativePath);
};
