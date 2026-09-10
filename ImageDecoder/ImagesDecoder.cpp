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

#include <algorithm>
#include <cctype>
#include <string>

bool ImagesDecoder::Write(const nsBitmapData &bitmap, const char *path, int quality) const {
    if (!path || bitmap.GetWidth() <= 0 || bitmap.GetHeight() <= 0 || !bitmap.GetData()) {
        Log::Error("Invalid image write arguments");
        return false;
    }
    std::string extension(path);
    const auto dot = extension.find_last_of('.');
    extension = dot == std::string::npos ? "" : extension.substr(dot);
    std::transform(extension.begin(), extension.end(), extension.begin(),
                   [](unsigned char c) { return char(std::tolower(c)); });
    for (const auto *decoder : _decoders) {
        if (!decoder->SupportsExtension(extension.c_str())) continue;
        if (decoder->Write(bitmap, path, quality)) return true;
        Log::Error("Unable to write image '%s'", path);
        return false;
    }
    Log::Error("Unsupported image extension: '%s'", path);
    return false;
}
