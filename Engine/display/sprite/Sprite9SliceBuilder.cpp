//
// Created by Roman on 3/7/2025.
//

#include "Sprite9SliceBuilder.h"
#include "Sprite9Slice.h"
#include "RenManager.h"
#include "Core/ParserUtils.h"

nsVisualObject2d *nsSprite9SliceBuilder::CreateDefault() {
    return new nsSprite9Slice();
}

bool nsSprite9SliceBuilder::Parse(script_state_t *ss, nsVisualObject2d *o, nsVisualCreationContext2d *context) {
    if (!nsVisualBuilder2d::Parse(ss, o, context)) {
        return false;
    }

    auto *sprite = Cast<nsSprite9Slice>(o);
    if (!sprite) {
        return false;
    }

    auto device = nsRenDevice::Shared()->Device();

    if (ps_var_begin(ss, "texture")) {
        sprite->desc.tex = device->TextureLoad(ps_var_str(ss), false);
    }

    auto state = device->StateLoad(ParseString(ss, "renState"));
    if (state) {
        sprite->renState = state;
    }

    if (ps_var_begin(ss, "width")) {
        sprite->desc.SetWidth(ps_var_f(ss));
    }

    if (ps_var_begin(ss, "height")) {
        sprite->desc.SetHeight(ps_var_f(ss));
    }

    if (ps_var_begin(ss, "color")) {
        ps_var_4f(ss, sprite->desc.color);
    }

    if (ps_block_begin(ss, "grid")) { //texture coord space - from top to bottom
        sprite->desc.SetGrid(
                ParseFloat(ss, "xMin"),
                ParseFloat(ss, "xMax"),
                ParseFloat(ss, "yMin"),
                ParseFloat(ss, "yMax"));

        ps_block_end(ss);
    }


    return true;
}
