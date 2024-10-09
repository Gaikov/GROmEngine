//
// Created by Roman on 8/1/2024.
//

#include "LayoutAnchor.h"

nsLayoutAnchor::nsLayoutAnchor() : type(NONE), value(0) {

}

bool nsLayoutAnchor::IsUsed() const {
    return type != NONE;
}

float nsLayoutAnchor::Compute(float size) const {
    if (type == PERCENT) {
        return size * value;
    } else if (type == NUMBER) {
        return value;
    }
    return 0;
}

void nsLayoutAnchor::Set(nsLayoutAnchor::nsAnchorType anchorType, float anchorValue) {
    type = anchorType;
    value = anchorValue;
}

void nsLayoutAnchor::SetNumber(float n) {
    Set(NUMBER, n);
}
