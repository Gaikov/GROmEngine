//
// Created by Roman on 10/19/2024.
//

#include "GroupLayoutBuilder.h"
#include "display/layouts/GroupLayout.h"
#include "Core/ParserUtils.h"

nsVisualObject2d *nsGroupLayoutBuilder::CreateDefault() {
    return new nsGroupLayout();
}

bool nsGroupLayoutBuilder::Parse(script_state_t *ss, nsVisualObject2d *object, nsVisualCreationContext2d *context) {
    if (!nsVisualContainerBuilder::Parse(ss, object, context)) {
        return false;
    }

    auto g = Cast<nsGroupLayout>(object);
    if (!g) {
        return false;
    }

    g->drawFrame = ParseFloat(ss, "drawFrame") > 0;
    return true;
}
