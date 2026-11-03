#include "Package.h"

#include <algorithm>
#include <cstring>
#include <filesystem>
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

bool IsLogicalPath(const char *name) {
    if (nsFilePath::IsAbsolute(name)) return false;
    const auto path = std::filesystem::path(nsFilePath(name).AsChar());
    if (path.has_root_path()) return false;
    for (const auto &part : path) {
        if (part == "..") return false;
    }
    return true;
}

#ifdef _WIN32
constexpr auto CASE_INSENSITIVE_PATHS = true;
#else
constexpr auto CASE_INSENSITIVE_PATHS = false;
#endif

} // namespace

//---------------------------------------------------------
// nsPackage::GetLogicalPath: only Internal filesystem aliases become logical.
//---------------------------------------------------------
bool nsPackage::GetLogicalPath(const char *fileName, nsString &result) const {
    if (!StrCheck(fileName)) return false;
    result = fileName;
    if (nsFilePath::IsAbsolute(fileName)) {
        const auto internal = App_GetPlatform()->GetInternalPath(".");
        nsFilePath root("");
        nsFilePath canonical("");
        if (!internal.IsEmpty() && internal.GetCanonical(root)
            && nsFilePath(fileName).GetCanonical(canonical)) {
            if (canonical.IsWithin(root, CASE_INSENSITIVE_PATHS)) {
                result = root.GetRelativePath(canonical);
            } else {
                const nsFilePath lexical(std::filesystem::path(fileName).lexically_normal().generic_string().c_str());
                const nsFilePath lexicalRoot(std::filesystem::path(internal.ToAbsolute().AsChar())
                    .lexically_normal().generic_string().c_str());
                // Validate escaped Internal aliases through the platform; unconfigured desktop storage stays native.
                if ((lexical.IsWithin(lexicalRoot, CASE_INSENSITIVE_PATHS)
                    || lexical.IsWithin(root, CASE_INSENSITIVE_PATHS))
                    && App_GetPlatform()->GetInternalPath(root.GetRelativePath(canonical)).IsEmpty()) return false;
            }
        }
    } else if (IsLogicalPath(fileName)) {
        result = std::filesystem::path(nsFilePath(fileName).AsChar()).lexically_normal().generic_string();
    }
    return true;
}

//---------------------------------------------------------
// nsPackage::ResolveFilePath: the winning native file, never a write target.
//---------------------------------------------------------
bool nsPackage::ResolveFilePath(const char *fileName, nsFilePath &result) const {
    nsString logical;
    if (!GetLogicalPath(fileName, logical)) return false;
    if (IsLogicalPath(logical)) {
        const auto internal = App_GetPlatform()->GetInternalPath(logical);
        if (internal.IsEmpty()) return false;
        if (internal.IsExists()) {
            return !internal.IsFolder() && internal.GetCanonical(result);
        }
    }
    const nsFilePath asset(logical);
    return asset.IsExists() && !asset.IsFolder() && asset.GetCanonical(result);
}

//---------------------------------------------------------
// nsPackage::OpenLooseFile: absence permits fallback, read errors do not.
//---------------------------------------------------------
bool nsPackage::OpenLooseFile(const char *logical, IDataReader *&reader) const {
    reader = nullptr;
    const auto internal = App_GetPlatform()->GetInternalPath(logical);
    if (internal.IsEmpty()) return true;
    if (internal.IsExists()) {
        if (!internal.IsFolder()) reader = App_GetPlatform()->InternalRead(logical);
        return true;
    }
    const nsFilePath asset(logical);
    if (asset.IsFolder()) return true;
    reader = App_GetPlatform()->FileRead(logical);
    if (asset.IsExists() || (reader && reader->IsValid())) return true;
    delete reader;
    reader = nullptr;
    return false;
}

bool nsPackage::Init() {
    Log::Info("...enum packs");
    nsFilePath::tList files;
    App_GetPlatform()->FolderListing(nsFilePath("./"), files);
    nsString *list = nullptr;
    auto count = 0;
    for (const auto &file : files) {
        if (!file.IsFolder() && file.CheckExtension("pak")) {
            const auto name = file.GetName();
            AddToArray(&list, count, name);
        }
    }
    if (!count) {
        Log::Warning("packs not found!");
        return true;
    }
    qsort(list, count, sizeof(nsString), FileCmp);
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
    nsString logical;
    if (!GetLogicalPath(fileName, logical)) return nullptr;
    nsFile *file = nullptr;
    if (IsLogicalPath(logical)) {
        IDataReader *reader = nullptr;
        const auto found = OpenLooseFile(logical, reader);
        IDataReader::sp_t owned(reader);
        file = found ? IDataReader::ReadBlob(owned) : LoadPackFile(logical);
    } else {
        // Explicit filesystem paths retain standalone absolute/parent access.
        IDataReader::sp_t reader = App_GetPlatform()->FileRead(fileName);
        file = IDataReader::ReadBlob(reader);
    }
    // Script loaders reject an empty file, but it still shadows lower sources.
    if (file && !file->GetSize()) {
        ReleaseFile(file);
        file = nullptr;
    }
    const nsBaseEvent event(FILE_LOADED_EVENT);
    Emmit(event);
    return file;
}

void nsPackage::ReleaseFile(nsFile *file) {
    delete file;
}

bool nsPackage::IsExists(const char *fileName) {
    nsString logical;
    if (!GetLogicalPath(fileName, logical)) return false;
    if (!IsLogicalPath(logical)) {
        IDataReader::sp_t reader = App_GetPlatform()->FileRead(fileName);
        return reader && reader->IsValid();
    }
    IDataReader *reader = nullptr;
    const auto found = OpenLooseFile(logical, reader);
    IDataReader::sp_t owned(reader);
    if (found) return owned && owned->IsValid();
    for (auto i = static_cast<int>(_packs.size()) - 1; i >= 0; --i) {
        if (FindPackFile(logical, i)) return true;
    }
    return false;
}

int nsPackage::EnumDirFiles(const char *dir, const char *type, nsString **list, bool withPacks) {
    *list = nullptr;
    nsString logical;
    if (!GetLogicalPath(StrCheck(dir) ? dir : ".", logical) || !StrCheck(type)) return 0;
    const auto virtualPaths = IsLogicalPath(logical);
    auto count = 0;
    const auto appendFiles = [&](const nsFilePath::tList &files, bool internal) {
        for (const auto &file : files) {
            if (file.IsFolder() || !file.CheckExtension(type)) continue;
            nsString path = file.GetPath();
            if (virtualPaths) {
                path = std::filesystem::path(nsFilePath(logical).ResolvePath(file.GetName()).AsChar())
                    .lexically_normal().generic_string();
            }
            if (internal && App_GetPlatform()->GetInternalPath(path).IsEmpty()) continue;
            if (!StrInList(*list, count, path)) AddToArray(&*list, count, path);
        }
    };
    if (virtualPaths) {
        const auto internal = App_GetPlatform()->GetInternalPath(logical);
        if (internal.IsEmpty()) return 0;
        nsFilePath::tList files;
        internal.Listing(files); // Native data directory, including on Android.
        appendFiles(files, true);
    }
    nsFilePath::tList files;
    App_GetPlatform()->FolderListing(nsFilePath(logical), files);
    appendFiles(files, false);
    if (virtualPaths && withPacks) {
        std::string prefix(logical.AsChar());
        if (prefix == "." || prefix == "./") prefix.clear();
        else if (!prefix.empty() && prefix.back() != '/') prefix += '/';
        std::transform(prefix.begin(), prefix.end(), prefix.begin(), ::tolower);
        for (auto p = static_cast<int>(_packs.size()) - 1; p >= 0; --p) {
            for (const auto &file : _packs[p].files) {
                if (strncmp(prefix.c_str(), file.filename, prefix.size()) != 0
                    || strchr(file.filename + prefix.size(), '/')
                    || !nsFilePath(file.filename).CheckExtension(type)) continue;
                const nsString name(file.filename);
                if (!StrInList(*list, count, name)) AddToArray(&*list, count, name);
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

const packFileDesc_t *nsPackage::FindPackFile(const char *fileName, int pack) const {
    if (!StrCheck(fileName)) return nullptr;
    for (auto &file : _packs[pack].files) {
        if (strcmp(file.filename, fileName) == 0) return &file;
    }
    return nullptr;
}

nsFile *nsPackage::LoadPackFile(const char *fileName) {
    const packFileDesc_t *desc = nullptr;
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
    if (!desc->size) {
        Log::Warning("zero-length file in pack '%s'!", fileName);
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
