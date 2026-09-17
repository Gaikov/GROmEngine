#include "UnpackStrategy.h"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <vector>

#include "Core/AssetCrypto.h"
#include "Core/PackArch.h"
#include "nsLib/log.h"

namespace fs = std::filesystem;

UnpackStrategy::UnpackStrategy(const char *packedFile, const char *targetFolder,
                               const char *propertiesFile) :
        _packedFile(packedFile), _targetFolder(targetFolder), _propertiesFile(propertiesFile) {
}

bool UnpackStrategy::Perform() {
    nsAssetCrypto::Key key;
    if (!nsAssetCrypto::LoadKey(_propertiesFile.string().c_str(), key)) {
        Log::Error("Invalid or missing assetEncryptionKey in properties file");
        return false;
    }

    std::ifstream input(_packedFile, std::ios::binary);
    packHeader_t header = {};
    if (!input.read(reinterpret_cast<char *>(&header), sizeof(header)) || !checkPackHeader(header)) {
        Log::Error("Invalid packed file format");
        return false;
    }

    std::vector<packFileDesc_t> files(header.filesCount);
    if (header.dirSize && !input.read(reinterpret_cast<char *>(files.data()), header.dirSize)) return false;

    nsAssetCrypto::Nonce dirNonce;
    nsAssetCrypto::Tag dirTag;
    std::copy(std::begin(header.dirNonce), std::end(header.dirNonce), dirNonce.begin());
    std::copy(std::begin(header.dirTag), std::end(header.dirTag), dirTag.begin());
    if (!nsAssetCrypto::Decrypt(files.data(), header.dirSize, key, dirNonce,
                                nsAssetCrypto::DirectoryAad(header.filesCount), dirTag)) {
        Log::Error("Archive directory authentication failed");
        return false;
    }

    for (const auto &file : files) {
        if (!memchr(file.filename, '\0', sizeof(file.filename))) return false;
        const fs::path relative(file.filename);
        if (relative.is_absolute() || relative.string().find("..") != std::string::npos) {
            Log::Error("Unsafe path in archive: %s", file.filename);
            return false;
        }

        std::vector<std::uint8_t> data(file.size);
        input.seekg(static_cast<std::streamoff>(file.offset));
        if (file.size && !input.read(reinterpret_cast<char *>(data.data()), file.size)) return false;

        nsAssetCrypto::Nonce nonce;
        nsAssetCrypto::Tag tag;
        std::copy(std::begin(file.nonce), std::end(file.nonce), nonce.begin());
        std::copy(std::begin(file.tag), std::end(file.tag), tag.begin());
        if (!nsAssetCrypto::Decrypt(data.data(), data.size(), key, nonce,
                                    nsAssetCrypto::FileAad(file.filename, file.size), tag)) {
            Log::Error("Asset authentication failed: %s", file.filename);
            return false;
        }

        const auto outputPath = _targetFolder / relative;
        std::error_code error;
        fs::create_directories(outputPath.parent_path(), error);
        std::ofstream output(outputPath, std::ios::binary | std::ios::trunc);
        if (!output || (file.size && !output.write(reinterpret_cast<char *>(data.data()), file.size))) {
            Log::Error("Can't write unpacked asset: %s", outputPath.string().c_str());
            return false;
        }
    }
    return true;
}
