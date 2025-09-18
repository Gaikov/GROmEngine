//
// Created by Roman on 4/25/2024.
//

#include "JPGDecoder.h"
#include "nsLib/log.h"
#include "jpeg/jpeglib.h"

bool JPGDecoder::IsSupport(nsFile *file) {

    jpeg_decompress_struct  cinfo = {};
    jpeg_error_mgr jerr = {};

    cinfo.err = jpeg_std_error(&jerr);

    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, file->GetData(), file->GetSize());

    auto result = jpeg_read_header(&cinfo, FALSE);
    jpeg_destroy_decompress(&cinfo);

    return result == JPEG_HEADER_OK;
}

nsBitmapData::tSP JPGDecoder::Decode(nsFile *file) {
    jpeg_decompress_struct  cinfo = {};
    jpeg_error_mgr jerr = {};

    cinfo.err = jpeg_std_error(&jerr);

    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, file->GetData(), file->GetSize());

    auto result = jpeg_read_header(&cinfo, FALSE);
    if (result != JPEG_HEADER_OK) {
        Log::Warning("Invalid JPEG header!");
        return nullptr;
    }

    if (!jpeg_start_decompress(&cinfo)) {
        jpeg_destroy_decompress(&cinfo);
        Log::Warning("Can't decompress JPEG!");
        return nullptr;
    }

    nsBitmapData::tSP bmData = nsBitmapData::Create((int)cinfo.output_width, (int)cinfo.output_height);

    int rowSize = bmData->GetWidth() * cinfo.output_components;
    assert(cinfo.output_components <= 4);

    auto *row = static_cast<unsigned char *>(malloc(rowSize));
    unsigned char *buffer[1] = {row};

    while (cinfo.output_scanline < cinfo.output_height) {
        auto line = bmData->GetScanLine((int)cinfo.output_scanline);
        jpeg_read_scanlines(&cinfo, buffer, 1);

        for (int i = 0; i < bmData->GetWidth(); i++) {
            auto &pixel = line[i];
            auto comps = row + cinfo.output_components * i;

            auto p = &pixel.r;
            pixel.a = 255;
            if (cinfo.output_components == 1) {
                p[0] = comps[0];
                p[1] = comps[0];
                p[2] = comps[0];
            } else {
                for (int c = 0; c < cinfo.output_components; c++) {
                    p[c] = comps[c];
                }
            }
        }
    }

    free(row);

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    return bmData;
}
