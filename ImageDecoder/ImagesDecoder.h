//
// Created by Roman on 4/25/2024.
//

#ifndef _GROM_IMAGESDECODER_H
#define _GROM_IMAGESDECODER_H


#include "Core/Blob.h"
#include "Core/BitmapData.h"
#include "IImageFormatDecoder.h"

class ImagesDecoder {
public:
    virtual ~ImagesDecoder();

    ImagesDecoder();

    nsBitmapData::tSP Decode(nsFile *file) const;

private:
    std::vector<IImageFormatDecoder*> _decoders;
};


#endif //_GROM_IMAGESDECODER_H
