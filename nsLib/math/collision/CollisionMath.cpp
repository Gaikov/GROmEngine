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

bool nsCollisionMath::RectToCircle(nsTransform2 &t1, const nsRect &r1, nsTransform2 &t2, float radius) {
    nsVec2  circle[2] = {
            {},
            {radius, 0}
    };

    for (auto &v : circle) {
        v = t1.ToLocal(t2.ToGlobal(v));
    }

    radius = (circle[1] - circle[0]).Length();
    auto pos = circle[0];

    auto center = r1.GetCenter();
    auto sizeX = r1.width / 2;
    auto sizeY = r1.height / 2;

    pos.x = fabsf(pos.x - center.x);
    pos.y = fabsf(pos.y - center.y);

    if (pos.x < sizeX + radius && pos.y < sizeY + radius) {
        if (pos.x < sizeX || pos.y < sizeY) {
            return true;
        }
        nsVec2 dir = {sizeX, sizeY};
        dir = pos - dir;
        return dir.Length() < radius;
    }

    return false;
}
