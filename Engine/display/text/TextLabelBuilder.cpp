//
// Created by Roman on 10/14/2024.
//

#include "TextLabelBuilder.h"
#include "TextLabel.h"
#include "renderer/font/FontsCache.h"
#include "Core/ParserUtils.h"
#include "RenManager.h"

nsVisualObject2d *nsTextLabelBuilder::CreateDefault() {
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
    if (ps_var_begin(ss, "color")) {
        ps_var_4f(ss, label->color);
    }

    label->drawFrame = ParseBool(ss, "drawFrame", label->drawFrame);
    label->hAlign = nsAlign::Parse(ss, "hAlign", label->hAlign);
    label->letterSpace = ParseFloat(ss, "letterSpace", label->letterSpace);

    return true;
}

