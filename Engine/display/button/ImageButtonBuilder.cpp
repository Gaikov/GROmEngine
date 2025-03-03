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

    ParseState(ss, "up", button->up);
    ParseState(ss, "over", button->over);
    ParseState(ss, "down", button->down);
    ParseState(ss, "disabled", button->disabled);

    if (ps_block_begin(ss, "label")) {
        button->text = ParseStrP(ss, "text");
        button->font = nsFontsCache::Shared()->LoadFont(ParseString(ss, "font"));
        ps_block_end(ss);
    }

    return nsVisualBuilder2d::Parse(ss, o, context);
}

void nsImageButtonBuilder::ParseState(script_state_t *ss, const char *name, nsSpriteDesc &desc) {
    auto dev = nsRenDevice::Shared()->Device();
    desc.tex = dev->TextureLoad(ParseString(ss, name), false);
    desc.ResetSize();
}


