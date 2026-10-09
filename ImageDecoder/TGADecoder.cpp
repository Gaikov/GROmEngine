// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
#include "TGADecoder.h"

#include "StbLegacyImage.h"
#include "nsLib/log.h"

bool TGADecoder::SupportsExtension( const char * ) const {
    return false;
}

bool TGADecoder::Write( const nsBitmapData &, const char *, int ) const {
    Log::Error( "Writing TGA images is not supported" );
    return false;
}

bool TGADecoder::IsSupport( nsFile *file ) {
    if ( !file || file->GetSize() < 18 ) return false;
    if ( file->GetData()[0] == 'B' && file->GetData()[1] == 'M' ) return false;
    return nsStbLegacyImage::IsDecodable( file );
}

nsBitmapData::tSP TGADecoder::Decode( nsFile *file ) {
    if ( !IsSupport( file ) ) return nullptr;
    return nsStbLegacyImage::Decode( file, "TGA" );
}
