//
// Created by Roman on 10/21/2024.
//

#include "TextButtonBuilder.h"
#include "TextButton.h"
#include "renderer/font/FontsCache.h"
#include "Core/ParserUtils.h"

nsVisualObject2d *nsTextButtonBuilder::CreateDefault() {
    return new nsTextButton();
}

bool nsTextButtonBuilder::Parse(script_state_t *ss, nsVisualObject2d *o, nsVisualCreationContext2d *context) {
    if (!nsBaseButtonBuilder::Parse(ss, o, context)) {
        return false;
    }

    auto button = Cast<nsTextButton>(o);
    if (!button) {
        return false;
    }

    auto font = nsFontsCache::Shared()->LoadFont(ParseString(ss, "font"));
    if (font) {
        button->font = font;
    }

    button->label = ParseStrP(ss, "label", button->label);

    nsColor color;
    if (ParseFloat4(ss, "labelColor", color)) {
        button->labelColor = color;
    }

    if (ParseFloat4(ss, "labelDisabledColor", color)) {
        button->labelDisabledColor = color;
    }

    nsVec2 scale;
    if (ParseFloat2(ss, "labelScale", scale)) {
        button->labelScale = scale;
    }

    return true;
}
