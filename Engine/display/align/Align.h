//
// Created by Roman on 8/8/2024.
//
#pragma once

class nsAlign final {
public:
    enum Type {
        BEGIN,
        CENTER,
        END
    };

    static float Compute(Type value, float contentSize, float areaSize);
};