#include "JPGDecoder.h"
#include "nsLib/log.h"
#include "jpeg/jpeglib.h"
#include <csetjmp>
#include <cstdio>
#include <cstring>
#include <vector>

namespace {
struct JpegError { jpeg_error_mgr base; jmp_buf jump; };
void FailJpeg(j_common_ptr info) {
    info->err->output_message(info);
    longjmp(reinterpret_cast<JpegError *>(info->err)->jump, 1);
}
}

bool JPGDecoder::IsSupport(nsFile *file) {
    return file && file->GetSize() >= 3 && file->GetData()[0] == 0xff &&
           file->GetData()[1] == 0xd8 && file->GetData()[2] == 0xff;
}

nsBitmapData::tSP JPGDecoder::Decode(nsFile *file) {
    if (!IsSupport(file)) return nullptr;
    // Heap state and pre-existing C++ objects remain valid after libjpeg longjmp.
    auto *info = new jpeg_decompress_struct{};
    JpegError error{};
    struct Pixels { nsBitmapData::tSP bitmap; std::vector<unsigned char> row; };
    auto *pixelsState = new Pixels;
    info->err = jpeg_std_error(&error.base);
    error.base.error_exit = FailJpeg;
    if (setjmp(error.jump)) {
        jpeg_destroy_decompress(info);
        delete info;
        delete pixelsState;
        return nullptr;
    }
    jpeg_create_decompress(info);
    jpeg_mem_src(info, file->GetData(), file->GetSize());
    jpeg_read_header(info, TRUE);
    info->out_color_space = JCS_RGB;
    jpeg_start_decompress(info);
    pixelsState->bitmap = nsBitmapData::Create(int(info->output_width), int(info->output_height));
    pixelsState->row.resize(size_t(info->output_width) * 3);
    auto &row = pixelsState->row;
    while (info->output_scanline < info->output_height) {
        auto *pixels = pixelsState->bitmap->GetScanLine(int(info->output_scanline));
        JSAMPROW scan = row.data();
        jpeg_read_scanlines(info, &scan, 1);
        for (unsigned x = 0; x < info->output_width; ++x)
            pixels[x] = {row[x*3], row[x*3+1], row[x*3+2], 255};
    }
    jpeg_finish_decompress(info);
    const bool valid = error.base.num_warnings == 0;
    jpeg_destroy_decompress(info);
    delete info;
    auto result = valid ? pixelsState->bitmap : nullptr;
    delete pixelsState;
    return result;
}

bool JPGDecoder::SupportsExtension(const char *extension) const {
    return !strcmp(extension, ".jpg") || !strcmp(extension, ".jpeg");
}

bool JPGDecoder::Write(const nsBitmapData &bitmap, const char *path, int quality) const {
    if (quality < 1 || quality > 100) return false;
    FILE *file = fopen(path, "wb");
    if (!file) return false;
    auto *info = new jpeg_compress_struct{};
    JpegError error{};
    std::vector<unsigned char> row(size_t(bitmap.GetWidth()) * 3);
    info->err = jpeg_std_error(&error.base);
    error.base.error_exit = FailJpeg;
    if (setjmp(error.jump)) {
        jpeg_destroy_compress(info);
        delete info;
        fclose(file);
        return false;
    }
    jpeg_create_compress(info);
    jpeg_stdio_dest(info, file);
    info->image_width = bitmap.GetWidth();
    info->image_height = bitmap.GetHeight();
    info->input_components = 3;
    info->in_color_space = JCS_RGB;
    jpeg_set_defaults(info);
    jpeg_set_quality(info, quality, TRUE);
    for (int c = 0; c < 3; ++c)
        info->comp_info[c].h_samp_factor = info->comp_info[c].v_samp_factor = 1;
    jpeg_start_compress(info, TRUE);
    while (info->next_scanline < info->image_height) {
        const auto *pixels = bitmap.GetData() + size_t(info->next_scanline) * bitmap.GetWidth();
        for (int x = 0; x < bitmap.GetWidth(); ++x) {
            row[x*3] = pixels[x].r; row[x*3+1] = pixels[x].g; row[x*3+2] = pixels[x].b;
        }
        JSAMPROW scan = row.data();
        jpeg_write_scanlines(info, &scan, 1);
    }
    jpeg_finish_compress(info);
    jpeg_destroy_compress(info);
    delete info;
    return fclose(file) == 0;
}
