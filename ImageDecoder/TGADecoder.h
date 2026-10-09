// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
#pragma once

#include "IImageFormatDecoder.h"

class TGADecoder final : public IImageFormatDecoder {
public:
    bool SupportsExtension( const char *extension ) const override;
    bool Write( const nsBitmapData &bitmap, const char *path, int quality ) const override;
    bool IsSupport( nsFile *file ) override;
    nsBitmapData::tSP Decode( nsFile *file ) override;
};
