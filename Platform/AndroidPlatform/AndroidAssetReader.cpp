//
// Created by Roman on 4/5/2024.
//

#include "AndroidAssetReader.h"
#include "nsLib/log.h"
#include "nsLib/StrTools.h"

AndroidAssetReader::AndroidAssetReader(AAssetManager *manager, const char *fileName) : _manager(
        manager), _position(0) {

    if (StrCheck(fileName)) {
        _fileName = fileName;
        _asset = AAssetManager_open(_manager, fileName, AASSET_MODE_STREAMING);
        if (!_asset) {
            Log::Warning("can't open android asset: %s", fileName);
        }
    } else {
        _fileName = "Invalid file name string";
    }
}

AndroidAssetReader::~AndroidAssetReader() {
    if (_asset) {
        AAsset_close(_asset);
    }
}

bool AndroidAssetReader::Read(void *data, uint size) {
    if (_asset) {
        auto readSize = AAsset_read(_asset, data, size);
        if (readSize > 0) {
            _position += readSize;
            return true;
        }
    }
    Log::Warning("Could not read asset data: %s", _fileName.c_str());
    return false;
}

bool AndroidAssetReader::Seek(long offset, int origin) {
    if (_asset) {
        _position = AAsset_seek(_asset, offset, origin);
        if (_position >= 0) {
            return true;
        }
    }
    Log::Warning("Could not seek asset: %s", _fileName.c_str());
    return false;
}

long AndroidAssetReader::Tell() {
    return _position;
}

bool AndroidAssetReader::IsValid() {
    return _asset;
}



