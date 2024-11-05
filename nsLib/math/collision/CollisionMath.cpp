//
// Created by Roman on 11/5/2024.
//

#include "CollisionMath.h"

bool nsCollisionMath::RectToRect(nsTransform2 &t1, const nsRect &r1, nsTransform2 &t2, const nsRect &r2) {
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

    nsRect  bounds1;
    bounds1.FromPoints(v1, 4);

    if (bounds1.Intersects(r2)) {

        nsVec2  v2[4] = {
                {r2.minX(), r2.minY()},
                {r2.minX(), r2.maxY()},
                {r2.maxX(), r2.maxY()},
                {r2.maxX(), r2.minY()},
        };

        for (auto &v : v2) {
            v = t1.ToLocal(t2.ToGlobal(v));
        }

        nsRect bounds2;
        bounds2.FromPoints(v2, 4);

        return bounds2.Intersects(r1);
    }

    return false;
}
