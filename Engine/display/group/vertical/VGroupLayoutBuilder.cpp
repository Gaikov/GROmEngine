//
// Created by Roman on 10/19/2024.
//

#include "VGroupLayoutBuilder.h"
#include "display/layouts/VGroupLayout.h"
#include "Core/ParserUtils.h"

nsVisualObject2d *nsVGroupLayoutBuilder::CreateDefault() {
    return new nsVGroupLayout;
}
bool nsVGroupLayoutBuilder::Parse(script_state_t *ss, nsVisualObject2d *object, nsVisualCreationContext2d *context) {
    if (!nsGroupLayoutBuilder::Parse(ss, object, context)) {
        return false;
    }

    auto g = Cast<nsVGroupLayout>(object);
    if (!g) {
        return false;
    }

    g->verticalAlign = nsAlign::Parse(ss, "verticalAlign", g->verticalAlign);
    g->gap = ParseFloat(ss, "gap", g->gap);

    return true;
}
