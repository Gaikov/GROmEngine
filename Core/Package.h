#pragma once

#include <vector>

#include "AssetCrypto.h"
#include "Blob.h"
#include "PackArch.h"
#include "nsLib/StrTools.h"
#include "nsLib/events/EventDispatcher.h"

class nsPackage : public nsEventDispatcher {
public:
    enum { FILE_LOADED_EVENT = 0 };

    bool Init();
    void Release();
    bool AddPack(const char *fileName);
    nsFile *LoadFile(const char *fileName);
    void ReleaseFile(nsFile *file);
    bool IsExists(const char *fileName);
    int EnumDirFiles(const char *dir, const char *type, nsString **list, bool withPacks = false);

    void SetEncryptionKey(const nsAssetCrypto::Key &key);
    void SetLoadPackedOnly(bool packed);

private:
    struct packDesc_t {
        nsString packName;
        std::vector<packFileDesc_t> files;
    };

    std::vector<packDesc_t> _packs;
    nsAssetCrypto::Key _encryptionKey = {};
    bool _hasEncryptionKey = false;
    bool _loadPacked = false;

    bool InitPack(const char *fileName);
    packFileDesc_t *FindPackFile(const char *fileName, int pack);
    nsFile *LoadPackFile(const char *fileName);
};

extern nsPackage g_pack;
