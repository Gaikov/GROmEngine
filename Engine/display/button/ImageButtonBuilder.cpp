//
// Created by Roman on 2/3/2025.
//

#include "ImageButtonBuilder.h"
#include "ImageButton.h"
#include "Core/ParserUtils.h"
#include "RenManager.h"
#include "renderer/font/FontsCache.h"

nsVisualObject2d *nsImageButtonBuilder::CreateDefault() {
    return new nsImageButton();
}

bool nsImageButtonBuilder::Parse(script_state_t *ss, nsVisualObject2d *o, nsVisualCreationContext2d *context) {
    auto dev = nsRenDevice::Shared()->Device();

    auto button = Cast<nsImageButton>(o);
    button->renState = dev->StateLoad(ParseString(ss, "renState"));

    button->up      .Parse(ss, context->assetsContext.get(), "up");
    button->over    .Parse(ss, context->assetsContext.get(), "over");
    button->down    .Parse(ss, context->assetsContext.get(), "down");
    button->disabled.Parse(ss, context->assetsContext.get(), "disabled");

    if (ps_block_begin(ss, "label")) {
        if (ps_var_begin(ss, "font")) {
            nsString    fileName = ps_var_str(ss);
            button->font = nsFontsCache::Shared()->LoadFont(fileName);
        }

        if (ps_var_begin(ss, "text")) {
            button->text = ps_var_str(ss);
        }

        if (ps_var_begin(ss, "color")) {
            ps_var_4f(ss, button->textColor);
        }

        button->AlignText(nsAlign::CENTER, nsAlign::CENTER);

        nsVec2 offset;
        if (ps_var_begin(ss, "offset")) {
            ps_var_2f(ss, offset);
            button->labelPos += offset;
        }

        ps_block_end(ss);
    }

    return nsVisualBuilder2d::Parse(ss, o, context);
}


