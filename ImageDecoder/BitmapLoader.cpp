//
// Created by Roman on 3/27/2024.
//

#include "BitmapLoader.h"
#include "nsLib/FilePath.h"
#include "Core/Package.h"
#include "ImagesDecoder.h"

nsBitmapData::tSP BitmapLoader::LoadFromFile(const char *filePath) {
    nsBitmapData::tSP bmData;

    nsFilePath path(filePath);
    auto              file = g_pack.LoadFile(filePath);
    if (!file)
    {
        return nullptr;
    }

    ImagesDecoder decoder;
    bmData = decoder.Decode(file);

    g_pack.ReleaseFile(file);
    return bmData;
}