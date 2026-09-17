#include "PackStrategy.h"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <limits>
#include <system_error>

#include "Options.h"
#include "nsLib/log.h"

namespace fs = std::filesystem;

namespace {

bool Write(std::fstream &stream, const void *data, std::size_t size) {
    if (!size) return true;
    stream.write(static_cast<const char *>(data), static_cast<std::streamsize>(size));
    return stream.good();
}

std::string Lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return value;
}

} // namespace

nsPackStrategy::nsPackStrategy(const Args &args) : _args(args) {
}

bool nsPackStrategy::Perform() {
    if (_args.Length() < 3) {
        Log::Error("Not enough parameters to pack data");
        return false;
    }

    const auto *keyFile = _args.GetByName(OPT_KEY_FILE);
    if (!keyFile || !nsAssetCrypto::LoadKey(keyFile, _key)) {
        Log::Error("Invalid or missing assetEncryptionKey in properties file");
        return false;
    }

    const fs::path sourceFolder(_args.GetParam(1));
    if (!fs::is_directory(sourceFolder)) {
        Log::Error("Invalid input folder: %s", sourceFolder.string().c_str());
        return false;
    }

    if (!Enumerate(sourceFolder)) return false;
    return WriteArchive(fs::path(_args.GetParam(2)));
}

bool nsPackStrategy::Enumerate(const fs::path &sourceFolder) {
    _files.clear();
    std::error_code error;
    for (fs::recursive_directory_iterator it(sourceFolder, error), end; it != end; it.increment(error)) {
        if (error) {
            Log::Error("Can't enumerate assets: %s", error.message().c_str());
            return false;
        }
        if (!it->is_regular_file()) continue;

        const auto relative = fs::relative(it->path(), sourceFolder, error);
        if (error || ShouldExclude(relative)) continue;

        auto archivePath = relative.generic_string();
        if (archivePath.size() >= PACK_MAX_PATH) {
            Log::Error("Asset path exceeds %i bytes: %s", PACK_MAX_PATH - 1, archivePath.c_str());
            return false;
        }

        const auto size = it->file_size(error);
        if (error || size > std::numeric_limits<std::uint32_t>::max()) {
            Log::Error("Invalid asset size: %s", archivePath.c_str());
            return false;
        }

        sourceFile_t file;
        file.absolutePath = it->path();
        file.archivePath = archivePath;
        strcpy(file.desc.filename, archivePath.c_str());
        file.desc.size = static_cast<std::uint32_t>(size);
        _files.push_back(std::move(file));
    }

    std::sort(_files.begin(), _files.end(), [](const auto &left, const auto &right) {
        return left.archivePath < right.archivePath;
    });
    Log::Info("Packing %u files", static_cast<unsigned>(_files.size()));
    return true;
}

bool nsPackStrategy::WriteArchive(const fs::path &outputFile) {
    std::error_code error;
    fs::create_directories(outputFile.parent_path(), error);
    if (error) {
        Log::Error("Can't create output folder: %s", error.message().c_str());
        return false;
    }

    auto temporaryFile = outputFile;
    temporaryFile += ".tmp";
    std::fstream output(temporaryFile, std::ios::binary | std::ios::in |
                                      std::ios::out | std::ios::trunc);
    if (!output) {
        Log::Error("Can't create archive: %s", temporaryFile.string().c_str());
        return false;
    }

    packHeader_t header = {};
    memcpy(header.id, PACK_ID, sizeof(header.id));
    header.version = PACK_VERSION;
    header.filesCount = static_cast<std::uint32_t>(_files.size());
    header.dirSize = header.filesCount * sizeof(packFileDesc_t);

    std::vector<packFileDesc_t> directory(header.filesCount);
    if (!Write(output, &header, sizeof(header)) ||
        !Write(output, directory.data(), header.dirSize)) return false;

    for (auto i = 0U; i < _files.size(); ++i) {
        auto &source = _files[i];
        auto &desc = source.desc;
        desc.offset = static_cast<std::uint64_t>(output.tellp());

        std::vector<std::uint8_t> data(desc.size);
        std::ifstream input(source.absolutePath, std::ios::binary);
        if (!input || (desc.size && !input.read(reinterpret_cast<char *>(data.data()), desc.size))) {
            Log::Error("Can't read asset: %s", source.archivePath.c_str());
            return false;
        }

        nsAssetCrypto::Nonce nonce;
        nsAssetCrypto::Tag tag;
        if (!nsAssetCrypto::GenerateNonce(nonce) ||
            !nsAssetCrypto::Encrypt(data.data(), data.size(), _key, nonce,
                                    nsAssetCrypto::FileAad(desc.filename, desc.size), tag)) {
            Log::Error("Can't encrypt asset: %s", source.archivePath.c_str());
            return false;
        }
        std::copy(nonce.begin(), nonce.end(), std::begin(desc.nonce));
        std::copy(tag.begin(), tag.end(), std::begin(desc.tag));
        if (!Write(output, data.data(), data.size())) return false;
        directory[i] = desc;
    }

    nsAssetCrypto::Nonce dirNonce;
    nsAssetCrypto::Tag dirTag;
    if (!nsAssetCrypto::GenerateNonce(dirNonce) ||
        !nsAssetCrypto::Encrypt(directory.data(), header.dirSize, _key, dirNonce,
                                nsAssetCrypto::DirectoryAad(header.filesCount), dirTag)) {
        Log::Error("Can't encrypt archive directory");
        return false;
    }
    std::copy(dirNonce.begin(), dirNonce.end(), std::begin(header.dirNonce));
    std::copy(dirTag.begin(), dirTag.end(), std::begin(header.dirTag));

    output.seekp(0);
    if (!Write(output, &header, sizeof(header)) ||
        !Write(output, directory.data(), header.dirSize)) return false;
    output.close();
    if (!output) return false;

    fs::remove(outputFile, error);
    error.clear();
    fs::rename(temporaryFile, outputFile, error);
    if (error) {
        Log::Error("Can't publish archive: %s", error.message().c_str());
        return false;
    }
    Log::Info("Archive created: %s", outputFile.string().c_str());
    return true;
}

bool nsPackStrategy::ShouldExclude(const fs::path &relativePath) {
    for (const auto &part : relativePath) {
        const auto name = part.string();
        if (!name.empty() && name[0] == '.') return true;
    }

    const auto name = Lower(relativePath.filename().string());
    const auto extension = Lower(relativePath.extension().string());
    if (extension == ".mis" || extension == ".log" || extension == ".txn" ||
        extension == ".bak" || extension == ".tmp") return true;
    if (name == "azangara.cfg" || name == "packs.sav" || name == "scores.dat") return true;
    return name.size() >= 8 && name.ends_with(".tmp.jpg");
}
