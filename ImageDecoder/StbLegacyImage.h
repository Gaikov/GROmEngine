// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
#pragma once

#include "Core/BitmapData.h"
#include "Core/Blob.h"

namespace nsStbLegacyImage {
    bool IsDecodable( nsFile *file );
    nsBitmapData::tSP Decode( nsFile *file, const char *formatName );
}
