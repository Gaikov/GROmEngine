//
// Created by Roman on 11/9/2024.
//

#include "VirtualCircleBuilder.h"
#include "VisualCircle.h"
#include "Core/ParserUtils.h"

nsVisualObject2d *nsVirtualCircleBuilder::CreateDefault() {
    return new nsVisualCircle();
}
bool nsVirtualCircleBuilder::Parse(script_state_t *ss, nsVisualObject2d *o, nsVisualCreationContext2d *context) {
    nsVisualBuilder2d::Parse(ss, o, context);

    auto circle = dynamic_cast<nsVisualCircle*>(o);

    circle->radius = ParseFloat(ss, "radius", circle->radius);
    if (ps_var_begin(ss, "color")) {
        ps_var_4f(ss, circle->color);
    }

    return true;
}


