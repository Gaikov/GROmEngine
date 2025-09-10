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

    if (ps_var_begin(ss, "drawFrame")) {
        g->drawFrame = ps_var_f(ss) > 0;
    }

    auto width = ParseFloat(ss, "width");
    if (width > 0) {
        g->SetWidth(width);
    }

    auto height = ParseFloat(ss, "height");
    if (height > 0) {
        g->SetHeight(height);
    }

    if (ps_var_begin(ss, "boundsType")) {
        const std::string boundsType = ps_var_str(ss);
        if (boundsType == "children") {
            g->boundsType = nsBaseLayout::CHILDREN;
        } else if (boundsType == "mixed") {
            g->boundsType = nsBaseLayout::MIXED;
        }
    }

    return true;
}
