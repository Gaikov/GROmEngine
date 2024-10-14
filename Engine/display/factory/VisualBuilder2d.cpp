//
// Created by Roman on 10/14/2024.
//
#include "VisualBuilder2d.h"
#include "Core/ParserUtils.h"

bool nsVisualBuilder2d::Parse(script_state_t *ss, nsVisualObject2d *o, nsVisualCreationContext2d *context) {

    o->origin.pos = {
            ParseFloat(ss, "x", o->origin.pos->x),
            ParseFloat(ss, "y", o->origin.pos->y)};

    o->origin.scale = {
            ParseFloat(ss, "sx", o->origin.scale->x),
            ParseFloat(ss, "sy", o->origin.scale->y)};

    if (ps_var_begin(ss, "angle")) {
        o->origin.angle = nsMath::ToRad(ps_var_f(ss));
    }

    return true;
}


