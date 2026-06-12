//
// Created by Roman on 6/9/2026.
//

#include "VisualPolygonBuilder.h"
#include "VisualPolygon.h"
#include "Core/ParserUtils.h"

nsVisualObject2d *nsVisualPolygonBuilder::CreateDefault() {
    return new nsVisualPolygon();
}

bool nsVisualPolygonBuilder::Parse(script_state_t *ss, nsVisualObject2d *o, nsVisualCreationContext2d *context) {
    nsVisualBuilder2d::Parse(ss, o, context);

    auto polygon = Cast<nsVisualPolygon>(o);
    if (!polygon) {
        return false;
    }

    if (ps_var_begin(ss, "color")) {
        ps_var_4f(ss, polygon->color);
    }

    polygon->points.clear();
    if (ps_var_begin(ss, "point")) {
        do {
            float v[2] = {};
            ps_var_2f(ss, v);
            polygon->points.emplace_back(v[0], v[1]);
        } while (ps_var_next(ss));
    }

    return true;
}
