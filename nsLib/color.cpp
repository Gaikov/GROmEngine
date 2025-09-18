#include "color.h"

const nsColor nsColor::white = nsColor( 1, 1, 1 );
const nsColor nsColor::black = nsColor( 0, 0, 0 );
const nsColor nsColor::red = nsColor( 1, 0, 0 );
const nsColor nsColor::green = nsColor( 0, 1, 0 );
const nsColor nsColor::blue = nsColor( 0, 0, 1 );
const nsColor nsColor::yellow = nsColor( 1, 1, 0 );
const nsColor nsColor::gray = nsColor( 0.5, 0.5, 0.5 );
const nsColor nsColor::darkGray = nsColor( 0.25, 0.25, 0.25 );

void nsColor::FromRGBA(uint32_t c) {
    FromBytes(
        (c >> 24) & 0xFF,
        (c >> 16) & 0xFF,
        (c >> 8) & 0xFF,
        c & 0xFF);
}

void nsColor::CopyTo(float c[4]) const {
    c[0] = r;
    c[1] = g;
    c[2] = b;
    c[3] = a;
}

void nsColor::FromBytes(byte r, byte g, byte b, byte a) {
    this->r = float(r) / 255.0f;
    this->g = float(g) / 255.0f;
    this->b = float(b) / 255.0f;
    this->a = float(a) / 255.0f;
}
