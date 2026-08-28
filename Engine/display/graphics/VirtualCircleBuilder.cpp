//
// Created by Roman on 11/9/2024.
//

#include "VirtualCircleBuilder.h"
#include "VisualCircle.h"
#include "Core/ParserUtils.h"

nsVisualObject2d *nsVirtualCircleBuilder::CreateDefault(nsVisualCreationContext2d *) {
    return new nsVisualCircle();
}
bool nsVirtualCircleBuilder::Parse(script_state_t *ss, nsVisualObject2d *o, nsVisualCreationContext2d *context) {
    if (!nsVisualBuilder2d::Parse(ss, o, context)) {
        return false;
    }

    const auto circle = Cast<nsVisualCircle>(o);
    if (!circle) {
        return false;
    }

    circle->radius = ParseFloat(ss, "radius", circle->radius);
    if (ps_var_begin(ss, "color")) {
        ps_var_4f(ss, circle->color);
    }

    return true;
}

bool nsVirtualCircleBuilder::SerializeProps(nsScriptSaver &saver, nsVisualObject2d *o,
                                            nsVisualCreationContext2d *context) {
    if (!nsVisualBuilder2d::SerializeProps(saver, o, context)) {
        return false;
    }

    const auto circle = Cast<nsVisualCircle>(o);
    if (!circle) {
        return false;
    }

    saver.VarFloat("radius", circle->radius, 100);
    saver.VarFloat4("color", circle->color, nsColor::white);
    return true;
}

