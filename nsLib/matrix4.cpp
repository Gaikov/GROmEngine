#include "matrix4.h"

#include <algorithm>
#include <cmath>
#include <limits>

const nsMatrix nsMatrix::identity = nsMatrix(1 );

bool nsMatrix::Inverse(nsMatrix &result) const {
    double work[4][8]{};
    double rowScale[4]{};
    for (auto row = 0; row < 4; ++row) {
        for (auto column = 0; column < 4; ++column) {
            const auto value = m[row][column];
            if (!std::isfinite(value)) return false;
            work[row][column] = value;
            rowScale[row] = std::max(rowScale[row], std::abs(double(value)));
        }
        if (rowScale[row] == 0) return false;
        work[row][row + 4] = 1;
    }

    // Gauss–Jordan elimination with scaled partial pivoting.
    for (auto column = 0; column < 4; ++column) {
        auto pivot = column;
        for (auto row = column + 1; row < 4; ++row) {
            if (std::abs(work[row][column]) / rowScale[row]
                > std::abs(work[pivot][column]) / rowScale[pivot]) {
                pivot = row;
            }
        }
        if (std::abs(work[pivot][column]) <= rowScale[pivot] * std::numeric_limits<double>::epsilon())
            return false;
        if (pivot != column) {
            std::swap(work[column], work[pivot]);
            std::swap(rowScale[column], rowScale[pivot]);
        }

        const auto divisor = work[column][column];
        for (auto &value : work[column]) value /= divisor;
        for (auto row = 0; row < 4; ++row) {
            if (row == column) continue;
            const auto factor = work[row][column];
            for (auto entry = 0; entry < 8; ++entry)
                work[row][entry] -= factor * work[column][entry];
        }
    }

    nsMatrix inverse;
    for (auto row = 0; row < 4; ++row) {
        for (auto column = 0; column < 4; ++column) {
            const auto value = work[row][column + 4];
            if (!std::isfinite(value) || std::abs(value) > std::numeric_limits<float>::max())
                return false;
            inverse.m[row][column] = static_cast<float>(value);
        }
    }
    result = inverse;
    return true;
}
