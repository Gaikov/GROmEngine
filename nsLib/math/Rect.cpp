//
// Created by Roman on 6/26/2024.
//

#include "Rect.h"

bool nsRect::IsInside(const nsVec2 &pos) const {
    return nsMath::InRange(pos.x, x, EndX()) && nsMath::InRange(pos.y, y, EndY());
}

bool nsRect::Intersects(const nsRect &other) const {

    float totalWidth = width + other.width;
    float boundsWidth = std::max(other.maxX(), maxX()) - std::min(other.minX(), minX());

    if (totalWidth > boundsWidth) {
        float totalHeight = height + other.height;
        float boundsHeight = std::max(other.maxY(), maxY()) - std::min(other.minY(), minY());

        return totalHeight > boundsHeight;
    }

    return false;
}
