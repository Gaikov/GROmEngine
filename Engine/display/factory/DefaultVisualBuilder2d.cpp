//
// Created by Roman on 11/6/2025.
//

#include "DefaultVisualBuilder2d.h"
#include "Core/ParserUtils.h"
#include "display/container/VisualContainer2d.h"
#include "display/default/DefaultVisualObject.h"

nsVisualObject2d * nsDefaultVisualBuilder2d::CreateDefault() {
    return new nsDefaultVisualObject();
}

bool nsDefaultVisualBuilder2d::Parse(script_state_t *ss, nsVisualObject2d *o, nsVisualCreationContext2d *context) {
    if (!nsVisualBuilder2d::Parse(ss, o, context)) {
        return false;
    }

    const auto visual = Cast<nsDefaultVisualObject>(o);
    visual->SetTagName(ps_block_name(ss));

    return true;
}
