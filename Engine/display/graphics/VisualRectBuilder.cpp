//
// Created by Roman on 11/11/2024.
//

#include "VisualRectBuilder.h"
#include "VisualRect.h"
#include "Core/ParserUtils.h"

nsVisualObject2d *nsVisualRectBuilder::CreateDefault() {
    return new nsVisualRect();
}

bool nsVisualRectBuilder::Parse(script_state_t *ss, nsVisualObject2d *o, nsVisualCreationContext2d *context) {
    nsVisualBuilder2d::Parse(ss, o, context);

    auto rect = Cast<nsVisualRect>(o);
    if (!rect) {
        return false;
    }

    if (ps_var_begin(ss, "color")) {
        ps_var_4f(ss, rect->color);
    }

    auto &r = rect->rect;

    float pivot[2];
    if (ParseFloat2(ss, "pivot", pivot)) {
        r.x = -pivot[0];
        r.y = -pivot[1];
    }

    r.width = ParseFloat(ss, "width", r.width);
    r.height = ParseFloat(ss, "height", r.height);

    return true;
}
