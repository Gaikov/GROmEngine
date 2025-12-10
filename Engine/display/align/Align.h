//
// Created by Roman on 8/8/2024.
//
#pragma once

#include "Core/Parser.h"
#include "Core/ScriptSaver.h"

class nsAlign final {
public:
    enum Type {
        BEGIN,
        CENTER,
        END
    };

    static float Compute(Type value, float contentSize, float areaSize);
    static Type Parse(script_state_t *ss, const char *name, Type defValue);
    static void Save(const nsScriptSaver &saver, const char *name, Type value, Type defValue);
};
