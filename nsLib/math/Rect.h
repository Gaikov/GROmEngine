//
// Created by Roman on 6/26/2024.
//

#ifndef RECT_H
#define RECT_H

#include "nsLib/Vec2.h"

struct nsRect {
    float x = 0;
    float y = 0;
    float width = 0;
    float height = 0;

    inline float minX() const { return std::min(x, x + width); }
    inline float maxX() const { return std::max(x, x + width); }
    inline float minY() const { return std::min(y, y + height); }
    inline float maxY() const { return std::max(y, y + height); }

    inline void SetPos(const nsVec2 &pos) {
        x = pos.x;
        y = pos.y;
    }

    inline void SetSize(const nsVec2 &size) {
        width = size.x;
        height = size.y;
    }

    inline void setZero() {
        x = y = width = height = 0;
    }

    inline nsVec2 GetPos() const {
        return {x, y};
    }

    inline nsVec2 GetSize() const {
        return {width, height};
    }

    nsRect& operator += (const nsRect &other) {

        auto minx = std::min(minX(), other.minX());
        auto maxx = std::max(maxX(), other.maxX());

        auto miny = std::min(minY(), other.minY());
        auto maxy = std::max(maxY(), other.maxY());

        x = minx;
        width = maxx - minx;

        y = miny;
        height = maxy - miny;

        return *this;
    }

    inline float EndX() const { return x + width; }
    inline float EndY() const { return y + height; }

    bool IsInside(const nsVec2 &pos) const;
    bool Intersects(const nsRect& other) const;
};

#endif //RECT_H
