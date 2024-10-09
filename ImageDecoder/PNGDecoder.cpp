//
// Created by Roman on 4/25/2024.
//

#include "PNGDecoder.h"
#include "nsLib/log.h"
#include "png/png.h"

struct PNGData {
    nsFile *file = nullptr;
    size_t offset = 0;
};

bool PNGDecoder::IsSupport(nsFile *file) {
    return png_sig_cmp(file->GetData(), 0, 8) == 0;
}

nsBitmapData::tSP PNGDecoder::Decode(nsFile *file) {
    PNGData dataFile = {
            file, 0
    };

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                             nullptr,
                                             [](png_struct *png, const char *error) {
                                                 Log::Error(error);

                                             },
                                             [](png_struct *png, const char *warning) {
                                                 Log::Warning(warning);
                                             });
    if (!png) {
        Log::Error("Can't create PNG struct");
        return nullptr;
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        png_destroy_read_struct(&png, nullptr, nullptr);
        return nullptr;
    }

    png_set_read_fn(png, &dataFile, [](png_structp png, png_bytep data, size_t size) {
        auto pngData = static_cast<PNGData *>(png_get_io_ptr(png));
        auto source = pngData->file->GetData();
        memcpy(data, source + pngData->offset, size);
        pngData->offset += size;
    });

    png_read_info(png, info);

    auto width = png_get_image_width(png, info);
    auto height = png_get_image_height(png, info);
    auto color_type = png_get_color_type(png, info);
    auto bit_depth = png_get_bit_depth(png, info);

    if (bit_depth == 16)
        png_set_strip_16(png);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);

    // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png);

    if (png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);

    // These color_type don't have an alpha channel then fill it with 0xff.
    if (color_type == PNG_COLOR_TYPE_RGB ||
        color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);

    png_read_update_info(png, info);

    nsBitmapData::tSP bmData = nsBitmapData::Create((int) width, (int) height);

    for (int y = 0; y < height; y++) {
        png_read_row(png, (png_bytep) bmData->GetScanLine(y), nullptr);
    }

    png_destroy_read_struct(&png, &info, nullptr);
    return bmData;
}
