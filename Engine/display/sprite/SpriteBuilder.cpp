//
// Created by Roman on 10/14/2024.
//

#include "SpriteBuilder.h"
#include "display/Sprite.h"
#include "RenManager.h"
#include "Core/ParserUtils.h"

nsVisualObject2d *nsSpriteBuilder::Create(script_state_t *ss, nsVisualCreationContext2d *context) {
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

    auto state = dev->StateLoad(ParseString(ss, "renState"));
    if (state) {
        sprite->renState = state;
    }

    return true;
}
