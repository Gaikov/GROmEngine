//
// Created by Roman on 9/25/2024.
//

#include "TriMath.h"
#include "MathTools.h"

bool nsTriMath::PointInTriangle(const nsVec2 &pt, const nsVec2 &v1, const nsVec2 &v2, const nsVec2 &v3) {
    nsVec2 dir1 = v1 - pt;
    nsVec2 dir2 = v2 - pt;
    nsVec2 dir3 = v3 - pt;
    dir1.Norm();
    dir2.Norm();
    dir3.Norm();

    auto accAngle = acosf(nsMath::Clamp11(dir1.Dot(dir2)));
    accAngle += acosf(nsMath::Clamp11(dir2.Dot(dir3)));
    accAngle += acosf(nsMath::Clamp11(dir3.Dot(dir1)));
    return accAngle >= M_PI * 2.0 * 0.99f;
}
