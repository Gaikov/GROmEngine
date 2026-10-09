// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
#include <climits>
#include <cstddef>

#include "StbLegacyImage.h"
#include "nsLib/log.h"

#define STBI_ONLY_BMP
#define STBI_ONLY_TGA
#define STBI_NO_LINEAR
#define STBI_NO_STDIO
#define STB_IMAGE_IMPLEMENTATION
#include "ThirdParty/stb/stb_image.h"

namespace {
    bool ResolveImageInfo( nsFile *file, int &width, int &height ) {
        if ( !file || file->GetSize() == 0 || file->GetSize() > INT_MAX ) return false;

        auto channels = 0;
        if ( !stbi_info_from_memory( file->GetData(), static_cast<int>( file->GetSize() ),
                                    &width, &height, &channels ) ) return false;
        if ( width <= 0 || height <= 0 ) return false;

        const auto pixelCount = static_cast<size_t>( width ) * static_cast<size_t>( height );
        return pixelCount <= static_cast<size_t>( INT_MAX ) / sizeof( nsPixel );
    }
}

bool nsStbLegacyImage::IsDecodable( nsFile *file ) {
    auto width = 0;
    auto height = 0;
    return ResolveImageInfo( file, width, height );
}

nsBitmapData::tSP nsStbLegacyImage::Decode( nsFile *file, const char *formatName ) {
    auto width = 0;
    auto height = 0;
    if ( !ResolveImageInfo( file, width, height ) ) {
        const auto *reason = stbi_failure_reason();
        Log::Error( "Unable to decode %s image: %s", formatName,
                    reason ? reason : "invalid image data" );
        return nullptr;
    }

    auto sourceChannels = 0;
    auto *pixels = stbi_load_from_memory(
        file->GetData(), static_cast<int>( file->GetSize() ),
        &width, &height, &sourceChannels, STBI_rgb_alpha );
    if ( !pixels ) {
        const auto *reason = stbi_failure_reason();
        Log::Error( "Unable to decode %s image: %s", formatName,
                    reason ? reason : "invalid image data" );
        return nullptr;
    }

    auto result = nsBitmapData::Create( width, height );
    result->SetData( pixels );
    stbi_image_free( pixels );
    return result;
}
