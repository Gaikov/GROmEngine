//
// Created by Roman on 3/7/2025.
//

#include "Sprite9SliceBuilder.h"
#include "Sprite9Slice.h"
#include "RenManager.h"
#include "Core/ParserUtils.h"
#include "Engine/assets/VisualAssetsContext.h"

nsVisualObject2d *nsSprite9SliceBuilder::CreateDefault(nsVisualCreationContext2d *) {
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

    if (auto t = context->assetsContext->ParseTexture(ss, "texture")) {
        sprite->tex = t;  // nsProperty<ITexture*>
    }

    if (auto state = device->StateLoad(context->assetsContext->ParseAssetPath(ss, "renState"))) {
        sprite->renState = state;
    }

    if (ps_var_begin(ss, "tex1"))       { nsVec2 v; ps_var_2f(ss, v); sprite->tex1 = v; }
    if (ps_var_begin(ss, "tex2"))       { nsVec2 v; ps_var_2f(ss, v); sprite->tex2 = v; }
    if (ps_var_begin(ss, "gridLeft"))   { sprite->gridLeft   = ps_var_f(ss); }
    if (ps_var_begin(ss, "gridRight"))  { sprite->gridRight  = ps_var_f(ss); }
    if (ps_var_begin(ss, "gridTop"))    { sprite->gridTop    = ps_var_f(ss); }
    if (ps_var_begin(ss, "gridBottom")) { sprite->gridBottom = ps_var_f(ss); }

    if (ps_var_begin(ss, "width"))  { sprite->SetWidth(ps_var_f(ss)); }
    if (ps_var_begin(ss, "height")) { sprite->SetHeight(ps_var_f(ss)); }

    if (ps_var_begin(ss, "color"))  { ps_var_4f(ss, sprite->color); }

    return true;
}

bool nsSprite9SliceBuilder::SerializeProps(nsScriptSaver &saver, nsVisualObject2d *o, nsVisualCreationContext2d *context) {
    if (!nsVisualBuilder2d::SerializeProps(saver, o, context)) {
        return false;
    }

    auto *sprite = Cast<nsSprite9Slice>(o);
    if (!sprite) {
        return false;
    }

    auto dev = nsRenDevice::Shared()->Device();

    if (const auto t = sprite->tex.GetValue()) {
        context->assetsContext->SaveAssetPath(saver, "texture", dev->TextureGetPath(t));
    }

    if (sprite->renState) {
        context->assetsContext->SaveAssetPath(saver, "renState", dev->StateGetPath(sprite->renState));
    }

    const nsVec2 t1 = sprite->tex1;
    const nsVec2 t2 = sprite->tex2;
    if (t1 != nsVec2()) { saver.VarFloat2("tex1", t1, nsVec2()); }
    if (t2 != nsVec2()) { saver.VarFloat2("tex2", t2, nsVec2()); }

    saver.VarFloat("gridLeft",   sprite->gridLeft,   0.0f);
    saver.VarFloat("gridRight",  sprite->gridRight,  0.0f);
    saver.VarFloat("gridTop",    sprite->gridTop,    0.0f);
    saver.VarFloat("gridBottom", sprite->gridBottom, 0.0f);

    saver.VarFloat("width",  sprite->width,  0.0f);
    saver.VarFloat("height", sprite->height, 0.0f);

    saver.VarFloat4("color", sprite->color, nsColor());

    return true;
}
