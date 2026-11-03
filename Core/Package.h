#pragma once

#include <vector>

#include "AssetCrypto.h"
#include "Blob.h"
#include "PackArch.h"
#include "nsLib/FilePath.h"
#include "nsLib/StrTools.h"
#include "nsLib/events/EventDispatcher.h"

struct IDataReader;

class nsPackage : public nsEventDispatcher {
public:
    enum { FILE_LOADED_EVENT = 0 };

    bool Init();
    void Release();
    bool ResolveFilePath(const char *fileName, nsFilePath &result) const;
    bool AddPack(const char *fileName);
    nsFile *LoadFile(const char *fileName);
    void ReleaseFile(nsFile *file);
    bool IsExists(const char *fileName);
    int EnumDirFiles(const char *dir, const char *type, nsString **list, bool withPacks = false);

    void SetEncryptionKey(const nsAssetCrypto::Key &key);

private:
    struct packDesc_t {
        nsString packName;
        std::vector<packFileDesc_t> files;
    };

    std::vector<packDesc_t> _packs;
    nsAssetCrypto::Key _encryptionKey = {};
    bool _hasEncryptionKey = false;

    bool InitPack(const char *fileName);
    bool GetLogicalPath(const char *fileName, nsString &result) const;
    // Returns true for a selected loose source, including an unreadable one.
    // The caller owns reader; nullptr with true blocks fallback.
    bool OpenLooseFile(const char *logical, IDataReader *&reader) const;
    const packFileDesc_t *FindPackFile(const char *fileName, int pack) const;
    nsFile *LoadPackFile(const char *fileName);
};

extern nsPackage g_pack;
