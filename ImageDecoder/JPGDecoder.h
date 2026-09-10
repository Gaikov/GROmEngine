//
// Created by Roman on 4/25/2024.
//

#ifndef _GROM_JPGDECODER_H
#define _GROM_JPGDECODER_H

#include "IImageFormatDecoder.h"

class JPGDecoder : public IImageFormatDecoder {
public:
    bool SupportsExtension(const char *extension) const override;
    bool Write(const nsBitmapData &bitmap, const char *path, int quality) const override;
    bool IsSupport(nsFile *file) override;

    nsBitmapData::tSP Decode(nsFile *file) override;
};


#endif //_GROM_JPGDECODER_H
