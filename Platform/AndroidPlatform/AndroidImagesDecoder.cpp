//
// Created by Roman on 3/19/2024.
//

#include <android/imagedecoder.h>
#include "AndroidImagesDecoder.h"
#include "Core/Package.h"
#include "nsLib/log.h"

nsBitmapData::tSP AndroidImagesDecoder::LoadFromFile(const char *fileName) {

    auto file= g_pack.LoadFile(fileName);
    if (!file) {
        return nullptr;
    }

    // Make a decoder to turn it into a texture
    AImageDecoder *pAndroidDecoder = nullptr;
    auto result = AImageDecoder_createFromBuffer(file->GetData(), file->GetSize(), &pAndroidDecoder);
    if (result != ANDROID_IMAGE_DECODER_SUCCESS) {
        Log::Error("Can't create image decoder: %s!", fileName);
        g_pack.ReleaseFile(file);
        return nullptr;
    }

    // make sure we get 8 bits per channel out. RGBA order.
    AImageDecoder_setAndroidBitmapFormat(pAndroidDecoder, ANDROID_BITMAP_FORMAT_RGBA_8888);

    // Get the image header, to help set everything up
    const AImageDecoderHeaderInfo *pAndroidHeader = nullptr;
    pAndroidHeader = AImageDecoder_getHeaderInfo(pAndroidDecoder);

    // important metrics for sending to GL
    auto width = AImageDecoderHeaderInfo_getWidth(pAndroidHeader);
    auto height = AImageDecoderHeaderInfo_getHeight(pAndroidHeader);
    auto stride = AImageDecoder_getMinimumStride(pAndroidDecoder);

    auto data = nsBitmapData::Create(width, height);

    // Get the bitmap data of the image
    auto size = stride * height;
    auto decodeResult = AImageDecoder_decodeImage(
            pAndroidDecoder,
            (void*)data->GetData(),
            stride,
            size);

    // cleanup helpers
    AImageDecoder_delete(pAndroidDecoder);
    g_pack.ReleaseFile(file);

    if (decodeResult != ANDROID_IMAGE_DECODER_SUCCESS) {
        Log::Error("Could not decode image: %s", fileName);
        return nullptr;
    }

    return data;
}