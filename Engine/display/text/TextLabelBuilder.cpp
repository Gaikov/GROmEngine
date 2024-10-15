//
// Created by Roman on 10/14/2024.
//

#include "TextLabelBuilder.h"
#include "TextLabel.h"
#include "renderer/font/FontsCache.h"
#include "Core/ParserUtils.h"
#include "RenManager.h"

nsVisualObject2d *nsTextLabelBuilder::Create(script_state_t *ss, nsVisualCreationContext2d *context) {
    return new nsTextLabel();
}

bool nsTextLabelBuilder::Parse(script_state_t *ss, nsVisualObject2d *object, nsVisualCreationContext2d *context) {
    if (!nsVisualBuilder2d::Parse(ss, object, context)) {
        return false;
    }

    auto label = dynamic_cast<nsTextLabel*>(object);
    auto font = nsFontsCache::Shared()->LoadFont(ParseString(ss, "font"));
    if (font) {
        label->font = font;
    }
    auto state = nsRenDevice::Shared()->Device()->StateLoad(ParseString(ss, "renState"));
    if (state) {
        label->renState = state;
    }

    label->text = ParseStrP(ss, "text", label->text);

    return true;
}
