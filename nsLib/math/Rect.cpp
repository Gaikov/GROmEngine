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

void nsRect::FromPoints(const nsVec2 points[], int count) {
    float minX;
    float maxX;
    float minY;
    float maxY;

    minX = maxX = points[0].x;
    minY = maxY = points[0].y;

    for (auto i = 1; i < count; i++) {
        auto &v = points[i];

        minX = std::min(minX, v.x);
        maxX = std::max(maxX, v.x);

        minY = std::min(minY, v.y);
        maxY = std::max(maxY, v.y);
    }

    x = minX;
    y = minY;
    width = maxX - minX;
    height = maxY - minY;
}
