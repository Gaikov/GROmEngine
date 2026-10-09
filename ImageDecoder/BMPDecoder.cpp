// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
#include "BMPDecoder.h"

#include "StbLegacyImage.h"
#include "nsLib/log.h"

bool BMPDecoder::SupportsExtension( const char * ) const {
    return false;
}

bool BMPDecoder::Write( const nsBitmapData &, const char *, int ) const {
    Log::Error( "Writing BMP images is not supported" );
    return false;
}

bool BMPDecoder::IsSupport( nsFile *file ) {
    return file && file->GetSize() >= 14
        && file->GetData()[0] == 'B' && file->GetData()[1] == 'M';
}

nsBitmapData::tSP BMPDecoder::Decode( nsFile *file ) {
    if ( !IsSupport( file ) ) return nullptr;
    return nsStbLegacyImage::Decode( file, "BMP" );
}
