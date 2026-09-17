#include "Package.h"

#include <algorithm>
#include <cstring>
#include <string>

#include "Engine/Platform.h"
#include "Memory.h"
#include "StructUt.h"
#include "nsLib/FilePath.h"
#include "nsLib/log.h"

nsPackage g_pack;

namespace {

bool StrInList(nsString *list, int count, const char *str) {
    for (auto i = 0; i < count; ++i) {
        if (list[i] == str) return true;
    }
    return false;
}

int FileCmp(const void *elem1, const void *elem2) {
    const auto *s1 = static_cast<const nsString *>(elem1);
    const auto *s2 = static_cast<const nsString *>(elem2);
    return strcmp(*s1, *s2);
}

} // namespace

bool nsPackage::Init() {
    nsString *list = nullptr;
    Log::Info("...enum packs");
    const auto count = EnumDirFiles("./", "pak", &list);
    if (!count) {
        Log::Warning("packs not found!");
        return !_loadPacked;
    }

    auto result = true;
    for (auto i = 0; i < count; ++i) result = AddPack(list[i]) && result;
    my_free(list);
    return result;
}

void nsPackage::Release() {
    if (!_packs.empty()) Log::Info("...release packs");
    _packs.clear();
    ClearHandlers();
}

bool nsPackage::AddPack(const char *fileName) {
    if (!fileName || !strlen(fileName)) return true;
    return InitPack(fileName);
}

nsFile *nsPackage::LoadFile(const char *fileName) {
    if (!fileName || !strlen(fileName)) return nullptr;

    nsFile *file = nullptr;
    if (!_loadPacked) {
        IDataReader::sp_t reader = App_GetPlatform()->FileRead(fileName);
        file = IDataReader::ReadBlob(reader);
    }
    if (!file) file = LoadPackFile(fileName);

    const nsBaseEvent event(FILE_LOADED_EVENT);
    Emmit(event);
    return file;
}

void nsPackage::ReleaseFile(nsFile *file) {
    delete file;
}

bool nsPackage::IsExists(const char *fileName) {
    if (!fileName || !strlen(fileName)) return false;
    if (!_loadPacked) {
        IDataReader::sp_t reader = App_GetPlatform()->FileRead(fileName);
        if (reader && reader->IsValid()) return true;
    }
    for (auto i = static_cast<int>(_packs.size()) - 1; i >= 0; --i) {
        if (FindPackFile(fileName, i)) return true;
    }
    return false;
}

int nsPackage::EnumDirFiles(const char *dir, const char *type, nsString **list, bool withPacks) {
    *list = nullptr;
    auto count = 0;
    nsFilePath folder(dir);
    nsFilePath::tList files;
    App_GetPlatform()->FolderListing(folder, files);
    for (auto &file : files) {
        if (!file.IsFolder() && file.CheckExtension(type)) AddToArray(&*list, count, file.GetPath());
    }

    if (withPacks) {
        for (auto p = static_cast<int>(_packs.size()) - 1; p >= 0; --p) {
            std::string currentDir = dir;
            std::transform(currentDir.begin(), currentDir.end(), currentDir.begin(), ::tolower);
            for (const auto &file : _packs[p].files) {
                if (strncmp(currentDir.c_str(), file.filename, currentDir.length()) == 0 &&
                    !strchr(file.filename + currentDir.length(), '/') &&
                    strstr(file.filename, StrPrintf(".%s", type))) {
                    nsString name(file.filename + currentDir.length());
                    if (!StrInList(*list, count, name)) AddToArray(&*list, count, name);
                }
            }
        }
    }
    if (count) qsort(*list, count, sizeof(nsString), FileCmp);
    return count;
}

bool nsPackage::InitPack(const char *fileName) {
    if (!_hasEncryptionKey) {
        Log::Error("Asset encryption key is not configured");
        return false;
    }

    IDataReader::sp_t reader = App_GetPlatform()->FileRead(fileName);
    if (!reader || !reader->IsValid()) {
        Log::Warning("can't open pack file: %s", fileName);
        return false;
    }

    packHeader_t header = {};
    if (!reader->Read(&header, sizeof(header)) || !checkPackHeader(header)) return false;

    std::vector<packFileDesc_t> files(header.filesCount);
    if (header.dirSize && !reader->Read(files.data(), header.dirSize)) {
        Log::Warning("Can't read encrypted pack directory");
        return false;
    }

    nsAssetCrypto::Nonce nonce;
    nsAssetCrypto::Tag tag;
    std::copy(std::begin(header.dirNonce), std::end(header.dirNonce), nonce.begin());
    std::copy(std::begin(header.dirTag), std::end(header.dirTag), tag.begin());
    if (!nsAssetCrypto::Decrypt(files.data(), header.dirSize, _encryptionKey, nonce,
                                nsAssetCrypto::DirectoryAad(header.filesCount), tag)) {
        Log::Error("Pack directory authentication failed: %s", fileName);
        return false;
    }

    for (const auto &file : files) {
        if (!memchr(file.filename, '\0', sizeof(file.filename))) {
            Log::Error("Invalid unterminated file name in pack: %s", fileName);
            return false;
        }
    }

    packDesc_t pack;
    pack.packName = fileName;
    pack.files = std::move(files);
    _packs.push_back(std::move(pack));
    Log::Info("Pack OK (%u files)", header.filesCount);
    return true;
}

packFileDesc_t *nsPackage::FindPackFile(const char *fileName, int pack) {
    if (!StrCheck(fileName)) return nullptr;
    for (auto &file : _packs[pack].files) {
        if (strcmp(file.filename, fileName) == 0) return &file;
    }
    return nullptr;
}

nsFile *nsPackage::LoadPackFile(const char *fileName) {
    packFileDesc_t *desc = nullptr;
    auto packIndex = -1;
    for (auto i = static_cast<int>(_packs.size()) - 1; i >= 0; --i) {
        desc = FindPackFile(fileName, i);
        if (desc) {
            packIndex = i;
            break;
        }
    }
    if (!desc) {
        Log::Warning("file desc not found in packs '%s'!", fileName);
        return nullptr;
    }

    IDataReader::sp_t reader = App_GetPlatform()->FileRead(_packs[packIndex].packName);
    if (!reader || !reader->IsValid()) return nullptr;

    auto file = new nsFile(desc->size);
    if (!reader->Seek(static_cast<long>(desc->offset), SEEK_SET) ||
        (desc->size && !reader->Read(file->GetData(), desc->size))) {
        Log::Warning("Can't read packed file '%s'", fileName);
        delete file;
        return nullptr;
    }

    nsAssetCrypto::Nonce nonce;
    nsAssetCrypto::Tag tag;
    std::copy(std::begin(desc->nonce), std::end(desc->nonce), nonce.begin());
    std::copy(std::begin(desc->tag), std::end(desc->tag), tag.begin());
    if (!nsAssetCrypto::Decrypt(file->GetData(), file->GetSize(), _encryptionKey, nonce,
                                nsAssetCrypto::FileAad(desc->filename, desc->size), tag)) {
        Log::Error("Packed file authentication failed: %s", fileName);
        delete file;
        return nullptr;
    }
    file->GetData()[desc->size] = 0;
    return file;
}

void nsPackage::SetEncryptionKey(const nsAssetCrypto::Key &key) {
    _encryptionKey = key;
    _hasEncryptionKey = true;
}

void nsPackage::SetLoadPackedOnly(bool packed) {
    _loadPacked = packed;
}
