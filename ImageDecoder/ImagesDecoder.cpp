//
// Created by Roman on 4/25/2024.
//

#include "ImagesDecoder.h"
#include "nsLib/log.h"
#include "PNGDecoder.h"
#include "JPGDecoder.h"

nsBitmapData::tSP ImagesDecoder::Decode(nsFile *file) const {

    for (auto it : _decoders) {
        if (it->IsSupport(file)) {
            return it->Decode(file);
        }
    }

    Log::Error("Image file format is not supported!");
    return nullptr;
}

ImagesDecoder::ImagesDecoder() {
    _decoders.push_back(new PNGDecoder());
    _decoders.push_back(new JPGDecoder());
}

ImagesDecoder::~ImagesDecoder() {
    for (auto it : _decoders) {
        delete it;
    }
}
