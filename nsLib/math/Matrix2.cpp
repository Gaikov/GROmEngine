//
// Created by Roman on 7/24/2024.
//

#include "Matrix2.h"
#include "StrTools.h"
#include "log.h"

void nsMatrix2::Transform(const nsVec2 &pos, const nsVec2 &scale, const float angle) {
    Identity();
    Translate(pos.x, pos.y);

    const auto s = sinf(-angle);
    const auto c = cosf(-angle);

    _11 = c * scale.x;
    _12 = s * scale.x;

    _21 = -s * scale.y;
    _22 = c * scale.y;
}

const nsMatrix2& nsMatrix2::Multiply(const nsMatrix2 &b) const {
    static nsMatrix2 res;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            float sum = 0;
            for (int k = 0; k < 3; k++)
                sum += m[i][k] * b.m[k][j];
            res.m[i][j] = sum;
        }
    }
    return res;
}

//https://ru.onlinemschool.com/math/assistance/matrix/inverse/?oms_all=a%3d%7b%7b10,9,8%7d,%7b7,6,5%7d,%7b4,3,1%7d%7d
bool nsMatrix2::Inverse(nsMatrix2 &inv) const {
    float det = ComputeDeterminant();
    if (det == 0) {
        return false;
    }

    auto idet = 1.0f/det;

    inv._11 = Minor<0, 0>() * idet;
    inv._21 = Minor<0, 1>() * idet;
    inv._31 = Minor<0, 2>() * idet;

    inv._12 = Minor<1, 0>() * idet;
    inv._22 = Minor<1, 1>() * idet;
    inv._32 = Minor<1, 2>() * idet;

    inv._13 = Minor<2, 0>() * idet;
    inv._23 = Minor<2, 1>() * idet;
    inv._33 = Minor<2, 2>() * idet;

    return true;
}

void nsMatrix2::LogInfo() {
    nsString    row;

    for (auto & i : m) {
        row = "";
        for (float j : i) {
            nsString cell;
            cell.Format("%f ", j);
            row += cell;
        }
        Log::Info(row);
    }

}

float nsMatrix2::ComputeDeterminant() const {
    return _11 * _22 * _33 + _12 * _23 * _31 + _13 * _21 * _32 -
        _13 * _22 * _31 - _11 * _23 * _32 - _12 * _21 * _33;
}

nsVec2 nsMatrix2::TransformPoint(const nsVec2 &pos) const {
    float out[3] = { 0, 0, 0};
    float in[3] = {pos.x, pos.y, 1};

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            out[i] += m[j][i] * in[j];
        }
    }

    return {out[0], out[1]};
}

nsVec2 nsMatrix2::TransformVector(const nsVec2 &v) const {
    float out[2] = { 0, 0};
    float in[2] = {v.x, v.y};

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            out[i] += m[j][i] * in[j];
        }
    }

    return {out[0], out[1]};
}

nsVec2 nsMatrix2::Multiply(const nsVec2 &pos) const {
    float out[3] = { 0, 0, 0};
    float in[3] = {pos.x, pos.y, 1};

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            out[i] += m[i][j] * in[j];
        }
    }

    return {out[0], out[1]};
}
