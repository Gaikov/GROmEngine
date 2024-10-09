//
// Created by Roman on 4/5/2024.
//

#ifndef _GROM_ANDROIDASSETREADER_H
#define _GROM_ANDROIDASSETREADER_H

#include <android/asset_manager.h>
#include "Core/DataWriter.h"
#include <string>

class AndroidAssetReader : public IDataReader {
public:
    AndroidAssetReader(AAssetManager *manager, const char *fileName);

    ~AndroidAssetReader() override;

    bool Read(void *data, uint size) override;

    bool Seek(long offset, int origin) override;

    long Tell() override;

    bool IsValid() override;

private:
    AAssetManager *_manager;
    AAsset *_asset;
    std::string _fileName;
    off_t _position;
};


#endif //_GROM_ANDROIDASSETREADER_H
