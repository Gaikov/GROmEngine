//
// Created by Roman on 10/29/2024.
//

#include "VisualAnchorBuilder.h"
#include "VisualAnchor.h"
#include "Core/ParserUtils.h"

nsVisualObject2d *nsVisualAnchorBuilder::CreateDefault() {
    return new nsVisualAnchor();
}

bool nsVisualAnchorBuilder::Parse(script_state_t *ss, nsVisualObject2d *o, nsVisualCreationContext2d *context) {
    if (!nsVisualBuilder2d::Parse(ss, o, context)) {
        return false;
    }

    auto anchor = Cast<nsVisualAnchor>(o);
    if (!anchor) {
        return false;
    }

    anchor->size = ParseFloat(ss, "size", anchor->size);

    nsColor c;
    if (ParseColor(ss, "color", c)) {
        anchor->color = c;
    }

    return true;
}
