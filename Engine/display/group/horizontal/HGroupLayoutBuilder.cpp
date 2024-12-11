//
// Created by Roman on 12/11/2024.
//

#include "HGroupLayoutBuilder.h"
#include "display/layouts/HGroupLayout.h"
#include "Core/ParserUtils.h"

nsVisualObject2d *nsHGroupLayoutBuilder::CreateDefault() {
    return new nsHGroupLayout();
}
bool nsHGroupLayoutBuilder::Parse(script_state_t *ss, nsVisualObject2d *object, nsVisualCreationContext2d *context) {
    if (!nsGroupLayoutBuilder::Parse(ss, object, context)) {
        return false;
    }

    auto g = Cast<nsHGroupLayout>(object);
    if (!g) {
        return false;
    }

    g->horizontalAlign = nsAlign::Parse(ss, "horizontalAlign", g->horizontalAlign);
    g->gap = ParseFloat(ss, "gap", g->gap);

    return true;
}
