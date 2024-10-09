//
// Created by Roman on 6/26/2024.
//

#include "Rect.h"
#include "MathTools.h"

bool nsRect::IsInside(const nsVec2 &pos) const {
    return nsMath::InRange(pos.x, x, EndX()) && nsMath::InRange(pos.y, y, EndY());
}