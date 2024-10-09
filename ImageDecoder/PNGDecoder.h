//
// Created by Roman on 4/25/2024.
//

#pragma once
#include "IImageFormatDecoder.h"

class PNGDecoder : public IImageFormatDecoder {
public:
    bool IsSupport(nsFile *file) override;

    nsBitmapData::tSP Decode(nsFile *file) override;
};
