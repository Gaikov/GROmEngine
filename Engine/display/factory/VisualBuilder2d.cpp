//
// Created by Roman on 10/14/2024.
//
#include "VisualBuilder2d.h"
#include "Core/ParserUtils.h"

nsVisualObject2d *nsVisualBuilder2d::Create(script_state_t *ss, nsVisualCreationContext2d *context) {
    auto visual = context->CreateByID(ParseString(ss, "bindingId"));
    if (!visual) {
        visual = CreateDefault();
    }
    return visual;
}

bool nsVisualBuilder2d::Parse(script_state_t *ss, nsVisualObject2d *o, nsVisualCreationContext2d *context) {

    auto id = ParseStrP(ss, "id");
    if (id) {
        o->id = id;
    }

    ParseAnchor(ss, "xMin", o->xMin);
    ParseAnchor(ss, "yMin", o->yMin);
    ParseAnchor(ss, "xMax", o->xMax);
    ParseAnchor(ss, "yMax", o->yMax);

    o->origin.pos = {
            ParseFloat(ss, "x", o->origin.pos->x),
            ParseFloat(ss, "y", o->origin.pos->y)};

    o->origin.scale = {
            ParseFloat(ss, "sx", o->origin.scale->x),
            ParseFloat(ss, "sy", o->origin.scale->y)};

    if (ps_var_begin(ss, "angle")) {
        o->origin.angle = nsMath::ToRad(ps_var_f(ss));
    }

    o->visible = ParseBool(ss, "visible", o->visible);
    return true;
}

void nsVisualBuilder2d::ParseAnchor(script_state_t *ss, const char *name, nsLayoutAnchor &anchor) {
    float val[2];
    if (ParseFloat2(ss, name, val)) {
        anchor.value = val[0];
        anchor.type = val[1] > 0 ? nsLayoutAnchor::PERCENT : nsLayoutAnchor::NUMBER;
    }
}


