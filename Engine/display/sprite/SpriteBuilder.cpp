//
// Created by Roman on 10/14/2024.
//

#include "SpriteBuilder.h"
#include "Sprite.h"
#include "RenManager.h"
#include "Core/ParserUtils.h"

nsVisualObject2d *nsSpriteBuilder::CreateDefault() {
    return new nsSprite();
}

bool nsSpriteBuilder::Parse(script_state_t *ss, nsVisualObject2d *object, nsVisualCreationContext2d *context) {
    if (!nsVisualBuilder2d::Parse(ss, object, context)) {
        return false;
    }

    auto sprite = dynamic_cast<nsSprite *>(object);
    auto dev = nsRenDevice::Shared()->Device();

    auto tex = dev->TextureLoad(ParseString(ss, "texture"), false);
    if (tex) {
        sprite->desc.tex = tex;
        sprite->desc.ResetSize();
    }

    if (ps_var_begin(ss, "pivot")) {
        ps_var_2f(ss, sprite->desc.center);
    }

    if (ps_var_begin(ss, "size")) {
        ps_var_2f(ss, sprite->desc.size);
    }

    if (ps_var_begin(ss, "color")) {
        ps_var_4f(ss, sprite->desc.color);
    }

    if (ps_var_begin(ss, "tilesX")) {
        sprite->desc.tex2.x = ps_var_f(ss);
    }

    if (ps_var_begin(ss, "tilesY")) {
        sprite->desc.tex2.y = ps_var_f(ss);
    }

    auto state = dev->StateLoad(ParseString(ss, "renState"));
    if (state) {
        sprite->renState = state;
    }

    return true;
}


