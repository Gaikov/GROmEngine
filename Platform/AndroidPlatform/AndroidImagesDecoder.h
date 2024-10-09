//
// Created by Roman on 3/19/2024.
//

#ifndef _GROM_ANDROIDIMAGESDECODER_H
#define _GROM_ANDROIDIMAGESDECODER_H

#include <android/asset_manager.h>
#include "Core/BitmapData.h"

class AndroidImagesDecoder {
public:
    static nsBitmapData::tSP LoadFromFile(const char *fileName);
};

#endif //_GROM_ANDROIDIMAGESDECODER_H
