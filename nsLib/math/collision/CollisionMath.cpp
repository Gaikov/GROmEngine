//
// Created by Roman on 11/5/2024.
//

#include "CollisionMath.h"

bool nsCollisionMath::RectToRect(nsTransform2 &t1, const nsRect &r1, nsTransform2 &t2, const nsRect &r2, nsVec2 &contact) {
    //r2 local space

    nsVec2  v1[4] = {
            {r1.minX(), r1.minY()},
            {r1.minX(), r1.maxY()},
            {r1.maxX(), r1.maxY()},
            {r1.maxX(), r1.minY()},
    };

    for (auto &v : v1) {
        v = t2.ToLocal(t1.ToGlobal(v));
    }

    float minX;
    float maxX;
    float minY;
    float maxY;

    minX = maxX = v1[0].x;
    minY = maxY = v1[0].y;

    for (auto i = 1; i < 4; i++) {
        auto &v = v1[i];

        minX = std::min(minX, v.x);
        maxX = std::max(maxX, v.x);

        minY = std::min(minY, v.y);
        maxY = std::max(maxY, v.y);
    }

    nsRect  bounds = {
            minX,
            minY,
            maxX - minX,
            maxY - minY
    };

    if (bounds.Intersects(r2)) {
        for (auto &v : v1) {
            if (r2.IsInside(v)) {
                contact = v;
                return true;
            }
        }

        nsVec2  v2[4] = {
                {r2.minX(), r2.minY()},
                {r2.minX(), r2.maxY()},
                {r2.maxX(), r2.maxY()},
                {r2.maxX(), r2.minY()},
        };


    }


    return false;
}
