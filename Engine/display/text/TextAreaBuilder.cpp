//
// Created by Roman on 5/7/2025.
//

#include "TextAreaBuilder.h"
#include "RenManager.h"
#include "Core/ParserUtils.h"
#include "TextArea.h"
#include "renderer/font/FontsCache.h"

nsVisualObject2d * nsTextAreaBuilder::CreateDefault(nsVisualCreationContext2d *) {
    return new nsTextArea();
}

bool nsTextAreaBuilder::Parse(script_state_t *ss, nsVisualObject2d *object, nsVisualCreationContext2d *context) {
    if (!nsVisualBuilder2d::Parse(ss, object, context)) {
        return false;
    }

    const auto area = Cast<nsTextArea>(object);
    if (!area) {
        return false;
    }

    const auto font = nsFontsCache::Shared()->LoadFont(context->assetsContext->ParseAssetPath(ss, "font"));
    if (font) {
        area->SetFont(font);
    }
    const auto state = nsRenDevice::Shared()->Device()->StateLoad(context->assetsContext->ParseAssetPath(ss, "renState"));
    if (state) {
        area->renState = state;
    }

    area->SetText(ParseStrP(ss, "text", area->GetText()));
    if (ps_var_begin(ss, "color")) {
        ps_var_4f(ss, area->color);
    }

    area->drawFrame = ParseBool(ss, "drawFrame", area->drawFrame);
    area->hAlign = nsAlign::Parse(ss, "hAlign", area->hAlign);
    area->SetLineSpace(ParseFloat(ss, "lineSpace", area->GetLineSpace()));
    area->textWidth = ParseFloat(ss, "width", area->textWidth);

    return true;
}
