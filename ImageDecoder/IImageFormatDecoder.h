//
// Created by Roman on 4/25/2024.
//

#ifndef _GROM_IIMAGEFORMATDECODER_H
#define _GROM_IIMAGEFORMATDECODER_H


#include "Core/Blob.h"
#include "Core/BitmapData.h"

class IImageFormatDecoder {

public:
    virtual ~IImageFormatDecoder() = default;

    virtual bool IsSupport(nsFile *file) = 0;
    virtual nsBitmapData::tSP Decode(nsFile *file) = 0;
};


#endif //_GROM_IIMAGEFORMATDECODER_H
