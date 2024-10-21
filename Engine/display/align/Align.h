//
// Created by Roman on 8/8/2024.
//
#pragma once

#include "Core/Parser.h"

class nsAlign final {
public:
    enum Type {
        BEGIN,
        CENTER,
        END
    };

    static float Compute(Type value, float contentSize, float areaSize);
    static Type Parse(script_state_t *ss, const char *name, Type defValue);
};