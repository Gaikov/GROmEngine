//
// Created by Roman on 10/21/2024.
//

#include "BaseButtonBuilder.h"
#include "BaseButton.h"
#include "Core/ParserUtils.h"

bool nsBaseButtonBuilder::Parse(script_state_t *ss, nsVisualObject2d *o, nsVisualCreationContext2d *context) {
    if (!nsVisualBuilder2d::Parse(ss, o, context)) {
        return false;
    }

    auto button = Cast<nsBaseButton>(o);
    if (!button) {
        return false;
    }

    button->minWidth = ParseFloat(ss, "minWidth", button->minWidth);

    return true;
}
