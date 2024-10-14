//
// Created by Roman on 10/14/2024.
//
#include "VisualBuilder2d.h"
#include "Core/ParserUtils.h"

bool nsVisualBuilder2d::Parse(script_state_t *ss, nsVisualObject2d *object, nsVisualCreationContext2d *context) {
    object->origin.pos = {ParseFloat(ss, "x"), ParseFloat(ss, "y")};
    object->origin.scale = {ParseFloat(ss, "sx", 1), ParseFloat(ss, "sy", 1)};
    object->origin.angle = nsMath::ToRad(ParseFloat(ss, "angle"));

    return true;
}


