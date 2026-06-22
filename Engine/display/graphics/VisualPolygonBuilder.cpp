//
// Created by Roman on 6/9/2026.
//

#include "VisualPolygonBuilder.h"
#include "VisualPolygon.h"
#include "Core/ParserUtils.h"

nsVisualObject2d *nsVisualPolygonBuilder::CreateDefault(nsVisualCreationContext2d *) {
    return new nsVisualPolygon();
}

bool nsVisualPolygonBuilder::Parse(script_state_t *ss, nsVisualObject2d *o, nsVisualCreationContext2d *context) {
    if (!nsVisualBuilder2d::Parse(ss, o, context)) {
        return false;
    }

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

bool nsVisualPolygonBuilder::SerializeProps(nsScriptSaver &saver, nsVisualObject2d *o,
                                            nsVisualCreationContext2d *context) {
    if (!nsVisualBuilder2d::SerializeProps(saver, o, context)) {
        return false;
    }

    const auto polygon = Cast<nsVisualPolygon>(o);
    if (!polygon) {
        return false;
    }

    saver.VarFloat4("color", polygon->color, nsColor());
    for (const auto &point: polygon->points) {
        saver.PrintVar("point", "%f %f", point.x, point.y);
    }

    return true;
}
