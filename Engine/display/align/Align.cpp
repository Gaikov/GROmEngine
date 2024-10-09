//
// Created by Roman on 8/8/2024.
//

#include "Align.h"

float nsAlign::Compute(const Type value, const float contentSize, const float areaSize) {
    if (value == BEGIN) {
        return 0;
    }
    if (value == CENTER) {
        return (areaSize - contentSize) / 2;
    }
    return areaSize - contentSize;
}
