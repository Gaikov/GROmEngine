//
// Created by Roman on 7/24/2024.
//

#pragma once
#include "nsLib/matrix4.h"

class nsMatrix2 {
public:
    union {
        float m[3][3] = {};

        struct {
            float a, b, x;
            float c, d, y;
            float tx, ty, w;
        };

        struct {
            float _11, _12, _13;
            float _21, _22, _23;
            float _31, _32, _33;
        };
    };

    nsMatrix2() {
        _11 = 1; _22 = _33 = 1;
        _12 = _13 = _21 = _23 = _31 = _32 = 0;
    }

    void Scale(const float x, const float y) {
        _11 = x; _22 = y;
        _33 = 1;
        _12 = _13 = _21 = _23 = _31 = _32 = 0;
    }

    void Identity() {
        Scale(1, 1);
    }

    void Translate(const float x, const float y) {
        _31 = x;
        _32 = y;
    }

    void ToMatrix3(nsMatrix &m) const {
        m.Identity();

        m._11 = _11; m._12 = _12; m._13 = _13;
        m._21 = _21; m._22 = _22; m._23 = _23;

        m._41 = _31; m._42 = _32;
    }

    template<int row, int col> float Minor() const {
        int row1 = (row + 1) % 3;
        int row2 = (row + 2) % 3;
        int col1 = (col + 1) % 3;
        int col2 = (col + 2) % 3;

        return m[row1][col1] * m[row2][col2] - m[row1][col2] * m[row2][col1];
    }

    void Transform(const nsVec2 &pos, const nsVec2 &scale, float angle);

    bool Inverse(nsMatrix2& inv) const;

    const nsMatrix2& Multiply(const nsMatrix2 &b) const;

    nsVec2 TransformPoint(const nsVec2 &pos) const;
    nsVec2 TransformVector(const nsVec2 &v) const;

    nsVec2 Multiply(const nsVec2 &pos) const;

    float ComputeDeterminant() const;

    void LogInfo();
};
