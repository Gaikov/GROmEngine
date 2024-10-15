//
// Created by Roman on 28/07/2019.
//

#include "BitmapData.h"
#include "nsLib/FilePath.h"

nsBitmapData::nsBitmapData(int w, int h) :
        _width(w),
        _height(h) {
    auto arraySize = w * h;
    _data = new nsPixel[arraySize];
    memset(_data, 0, sizeof(nsPixel) * arraySize);
}

nsBitmapData::~nsBitmapData() {
    delete[] _data;
}

int nsBitmapData::GetWidth() const {
    return _width;
}

int nsBitmapData::GetHeight() const {
    return _height;
}

const nsPixel *nsBitmapData::GetData() const {
    return _data;
}

void nsBitmapData::SetData(const void *data) {
    auto size = _width * _height * 4;
    memcpy(_data, data, size);
}

void nsBitmapData::SetScanLine(int y, const void *data) {
    auto line = GetScanLine(y);
    memcpy(line, data, sizeof(nsPixel) * _width);
}

nsPixel *nsBitmapData::GetScanLine(int y) {
    assert(y >= 0 && y < _height);
    return &_data[_width * y];
}

nsBitmapData::tSP nsBitmapData::Create(int width, int height) {
    return std::shared_ptr<nsBitmapData>(new nsBitmapData(width, height));
}

void nsBitmapData::PremultiplyAlpha() {
    for (auto x = 0; x < _width; x++) {
        for (auto y = 0; y < _height; y++) {
            auto p = &_data[y * _width + x];
            float a = float(p->a) / 255.0f;
            p->r = (unsigned char) (float(p->r) * a);
            p->g = (unsigned char) (float(p->g) * a);
            p->b = (unsigned char) (float(p->b) * a);
        }
    }
}
