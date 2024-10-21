//
// Created by Roman on 8/8/2024.
//

#include "Align.h"
#include "Core/ParserUtils.h"

float nsAlign::Compute(const Type value, const float contentSize, const float areaSize) {
    if (value == BEGIN) {
        return 0;
    }
    if (value == CENTER) {
        return (areaSize - contentSize) / 2;
    }
    return areaSize - contentSize;
}

nsAlign::Type nsAlign::Parse(script_state_t *ss, const char *name, Type defValue) {
    auto value = ParseStrP(ss, name);
    if (!value) {
        return defValue;
    }

    if (StrEqual(value, "end")) {
        return END;
    } else if (StrEqual(value, "center")) {
        return CENTER;
    }

    return BEGIN;
}
