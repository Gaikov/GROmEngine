#include "Package.h"
#include "Memory.h"
#include "nsLib/log.h"
#include "StructUt.h"
#include "nsLib/StrTools.h"
#include <dirent.h>
#include <string>
#include <algorithm>
#include "nsLib/FilePath.h"
#include "Engine/Platform.h"
#include "Crypt.h"

nsPackage g_pack;

//---------------------------------------------------------
// nsPackage::Init: 
//---------------------------------------------------------
bool nsPackage::Init() {
    nsString *list = nullptr;
    LogPrintf(PRN_ALL, "...enum packs\n");
    int count = EnumDirFiles("./", "pak", &list);
    if (!count)
        LogPrintf(PRN_ALL, "WARNING: packs not found!\n");
    else {
        for (int i = 0; i < count; i++)
            AddPack(list[i]);
        my_free(list);
    }
    return true;
}

//---------------------------------------------------------
// nsPackage::Release: 
//---------------------------------------------------------
void nsPackage::Release() {
    if (!m_packs.empty()) {
        Log::Info("...release packs");
        for (auto &pack: m_packs) {
            if (pack.files)
                my_free(pack.files);
        }
        m_packs.clear();
    }
}

//---------------------------------------------------------
// nsPackage::AddPack: 
//---------------------------------------------------------
bool nsPackage::AddPack(const char *fileName) {
    if (!fileName || !strlen(fileName)) return true;

    return InitPack(fileName);
}

//---------------------------------------------------------
// nsPackage::LoadFile: 
//---------------------------------------------------------
nsFile *nsPackage::LoadFile(const char *fileName) {
    if (!fileName || !strlen(fileName)) return nullptr;

    nsFile *file = nullptr;

    if (!_loadPacked) {
        IDataReader::sp_t reader = App_GetPlatform()->FileRead(fileName);
        file = IDataReader::ReadBlob(reader);
    }

    if (!file) {
        file = LoadPackFile(fileName);
    }

    const nsBaseEvent event(FILE_LOADED_EVENT);
    Emmit(event);
    return file;
}

//---------------------------------------------------------
// nsPackage::ReleaseFile: 
//---------------------------------------------------------
void nsPackage::ReleaseFile(nsFile *file) {
    delete file;
}

//---------------------------------------------------------
// nsPackage::IsExists: 
//---------------------------------------------------------
bool nsPackage::IsExists(const char *fileName) {
    if (!fileName || !strlen(fileName)) return false;

    IDataReader::sp_t reader = App_GetPlatform()->FileRead(fileName);
    if (reader && reader->IsValid()) {
        return true;
    } else {
        for (int i = (int) m_packs.size() - 1; i >= 0; i--)
            if (FindPackFile(fileName, i)) return true;
    }

    return false;
}

//---------------------------------------------------------
// nsPackage::EnumDirFiles: 
//---------------------------------------------------------
bool StrInList(nsString *list, int count, const char *str) {
    for (int i = 0; i < count; i++)
        if (list[i] == str) return true;
    return false;
}

//---------------------------------------------------------
int FileCmp(const void *elem1, const void *elem2) {
    const auto *s1 = (const nsString *) elem1;
    const auto *s2 = (const nsString *) elem2;
    return strcmp(*s1, *s2);
}

//---------------------------------------------------------
int nsPackage::EnumDirFiles(const char *dir, const char *type, nsString **list, bool withPacks) {
    *list = nullptr;
    int count = 0;

    nsFilePath folder(dir);
    nsFilePath::tList files;

    App_GetPlatform()->FolderListing(folder, files);
    for (auto &file: files) {
        if (!file.IsFolder() && file.CheckExtension(type)) {
            AddToArray(&*list, count, file.GetPath());
        }
    }

    if (withPacks) //TODO: need to check (does not work maybe)
    {
        for (int p = (int) m_packs.size() - 1; p >= 0; p--) {
            std::string cdir = dir;
            std::transform(cdir.begin(), cdir.end(), cdir.begin(), ::tolower);

            for (int i = 0; i < m_packs[p].count; i++) {
                if (strncmp(cdir.c_str(), m_packs[p].files[i].filename, cdir.length()) == 0 &&
                    !strchr(m_packs[p].files[i].filename + cdir.length(), '/') &&
                    strstr(m_packs[p].files[i].filename, StrPrintf(".%s", type))) {
                    nsString name(m_packs[p].files[i].filename + cdir.length());
                    if (!StrInList(*list, count, name))
                        AddToArray(&*list, count, name);
                }
            }
        }
    }

    if (count)
        qsort(*list, count, sizeof(nsString), FileCmp);

    return count;
}

//---------------------------------------------------------
// nsPackage::InitPack: 
//---------------------------------------------------------
bool nsPackage::InitPack(const char *fileName) {
    LogPrintf(PRN_ALL, "checking '%s'... ", fileName);
    IDataReader::sp_t packReader = App_GetPlatform()->FileRead(fileName);

    unsigned int filesCount;
    if (!packReader->IsValid()) {
        Log::Warning("can't open pack file: %s", fileName);
        return false;
    }

    packHeader_t ph;
    if (!packReader->Read(&ph, sizeof(packHeader_t))) {
        Log::Warning("Can't read pack header!");
        return false;
    }

    if (!checkPackHeader(ph)) {
        return false;
    }

    auto files = (packFileDesc_t *) my_malloc(ph.dir_size);
    if (!files) {
        Log::Error("allocate pack dir!");
        return false;
    }

    if (!packReader->Read(files, ph.dir_size)) {
        my_free(files);
        Log::Warning("read pack dir!");
        return false;
    }

    filesCount = ph.dir_size / sizeof(packFileDesc_t);
    Log::Info("OK! (%i - files)", filesCount);

    packDesc_t desc;
    desc.packName = fileName;
    desc.count = filesCount;
    desc.files = files;
    m_packs.push_back(desc);

    return true;
}

//---------------------------------------------------------
// nsPackage::FindPackFile: 
//---------------------------------------------------------
packFileDesc_t *nsPackage::FindPackFile(const char *fileName, int pack) {
    if (!StrCheck(fileName)) return nullptr;

    LogPrintf(PRN_DEV, "searching file desc '%s'\n", fileName);
    for (int i = 0; i < m_packs[pack].count; i++)
        if (strcmp(m_packs[pack].files[i].filename, fileName) == 0) {
            Log::Info("Found");
            return &m_packs[pack].files[i];
        }

    return nullptr;
}

//---------------------------------------------------------
// nsPackage::LoadPackFile: 
//---------------------------------------------------------
nsFile *nsPackage::LoadPackFile(const char *fileName) {
    packFileDesc_t *fd = nullptr;
    int packIndex;

    for (int i = (int) m_packs.size() - 1; i >= 0; i--) {
        fd = FindPackFile(fileName, i);
        if (fd) {
            packIndex = i;
            break;
        }
    }

    if (!fd) {
        Log::Warning("file desc not found in packs '%s'!", fileName);
        return nullptr;
    }

    IDataReader::sp_t packReader = App_GetPlatform()->FileRead(m_packs[packIndex].packName);
    if (!packReader->IsValid()) {
        return nullptr;
    }

    auto file = new nsFile(fd->size);
    if (!file->GetData()) {
        Log::Error("Can't allocate file '%s'!", fileName);
        delete file;
        return nullptr;
    }

    if (!packReader->Seek((long) fd->offset, SEEK_SET)) {
        Log::Warning("Can't seek packed file '%s'!", fileName);
        delete file;
        return nullptr;
    }

    if (!packReader->Read(file->GetData(), file->GetSize())) {
        Log::Warning("Can't read packed file '%s'", fileName);
        delete file;
        return nullptr;
    }

    if (!_decryptionKey.IsEmpty()) {
        nsCrypt::XorEncode(file->GetData(), file->GetSize(), _decryptionKey);
    } else {
        if (strstr(fileName, ".txt"))
            DecodeFile(file);
    }

    file->GetData()[fd->size] = 0; //for parsing file as a string
    return file;
}

//---------------------------------------------------------
// nsPackage::DecodeFile: 
//---------------------------------------------------------
void nsPackage::DecodeFile(nsFile *file) {
    if (!file || !file->GetData())
        return;

    uchar *data = file->GetData();
    uchar lo, hi;
    for (uint i = 0; i < file->GetSize(); i++, data++) {
        lo = (*data) & 0x0F;
        hi = (*data) & 0xF0;
        *data = (lo << 4) | (hi >> 4);
    }
}

void nsPackage::SetPassKey(const char *key) {
    assert(strlen(key) < nsString::MAX_SIZE - 1);
    _decryptionKey = key;
}

void nsPackage::SetLoadPackedOnly(bool packed) {
    _loadPacked = packed;
}
