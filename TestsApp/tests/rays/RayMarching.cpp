//
// Created by Roman on 8/2/2024.
//

#include "RayMarching.h"

nsBoxFunction::nsBoxFunction(float width, float height) : _width(width), _height(height) {

}

float nsBoxFunction::GetDistance(const nsVec2 &point) const {
    nsVec2  dir;
    dir.x = abs(point.x) - _width / 2;
    if (dir.x < 0) {
        dir.x = 0;
    }

    dir.y = abs(point.y) - _height / 2;
    if (dir.y < 0) {
        dir.y = 0;
    }
    return dir.Length();
}

float nsRayMarching::GetDistance(const nsSDFunction *func, const nsVec2 &start, const nsVec2 &dir, float maxDistance, int numSteps) {
    nsVec2  pos = start;
    float currDist = 0;
    for (int i = 0; i < numSteps; i++) {
        float dist = func->GetDistance(pos);
        if (dist > maxDistance) {
            return maxDistance;
        }

        currDist += dist;
        if (dist < 1) {
            return currDist;
        }
        pos += dir * dist;
    }

    return maxDistance;
}
