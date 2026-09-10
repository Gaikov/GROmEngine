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
    return file && file->GetSize() >= 8 && png_sig_cmp(file->GetData(), 0, 8) == 0;
}

nsBitmapData::tSP PNGDecoder::Decode(nsFile *file) {
    PNGData dataFile = {
            file, 0
    };

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                             nullptr,
                                             [](png_struct *png, const char *error) {
                                                 Log::Error("%s", error);
                                                 png_longjmp(png, 1);

                                             },
                                             [](png_struct *png, const char *warning) {
                                                 Log::Warning("%s", warning);
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

    auto *bitmap = new nsBitmapData::tSP;
    if (setjmp(png_jmpbuf(png))) {
        delete bitmap;
        png_destroy_read_struct(&png, &info, nullptr);
        return nullptr;
    }
    png_set_read_fn(png, &dataFile, [](png_structp png, png_bytep data, size_t size) {
        auto pngData = static_cast<PNGData *>(png_get_io_ptr(png));
        if (pngData->offset > pngData->file->GetSize() || size > pngData->file->GetSize() - pngData->offset)
            png_error(png, "Truncated PNG");
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

    *bitmap = nsBitmapData::Create((int) width, (int) height);

    for (int y = 0; y < height; y++) {
        png_read_row(png, (png_bytep) (*bitmap)->GetScanLine(y), nullptr);
    }

    png_read_end(png, info);
    png_destroy_read_struct(&png, &info, nullptr);
    auto result = *bitmap;
    delete bitmap;
    return result;
}

#include <cstdio>
#include <cstring>

bool PNGDecoder::SupportsExtension(const char *extension) const {
    return !strcmp(extension, ".png");
}

bool PNGDecoder::Write(const nsBitmapData &bitmap, const char *path, int) const {
    FILE *file = fopen(path, "wb");
    if (!file) return false;
    auto png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png) { fclose(file); return false; }
    auto info = png_create_info_struct(png);
    if (!info) { png_destroy_write_struct(&png, nullptr); fclose(file); return false; }
    if (setjmp(png_jmpbuf(png))) {
        png_destroy_write_struct(&png, &info);
        fclose(file);
        return false;
    }
    png_init_io(png, file);
    png_set_IHDR(png, info, bitmap.GetWidth(), bitmap.GetHeight(), 8,
                 PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);
    for (int y = 0; y < bitmap.GetHeight(); ++y)
        png_write_row(png, reinterpret_cast<png_const_bytep>(bitmap.GetData() + size_t(y) * bitmap.GetWidth()));
    png_write_end(png, info);
    png_destroy_write_struct(&png, &info);
    return fclose(file) == 0;
}
