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

    const auto sprite = dynamic_cast<nsSprite *>(object);
    const auto dev = nsRenDevice::Shared()->Device();

    auto assetPath = context->assetsContext->ParseAssetPath(ss, "texture");
    if (!assetPath.IsEmpty()) {
        if (const auto tex = dev->TextureLoad(assetPath, false)) {
            sprite->desc.tex = tex;
            sprite->desc.ResetSize();
        }
    }

    if (ps_var_begin(ss, "pivot")) {
        ps_var_2f(ss, sprite->desc.center);
    }

    if (ps_var_begin(ss, "size")) {
        ps_var_2f(ss, sprite->desc.size);
    }

    ParseColorExt(ss, "color", sprite->desc.color);

    if (ps_var_begin(ss, "tex1")) {
        ps_var_2f(ss, sprite->desc.tex1);
    }

    if (ps_var_begin(ss, "tilesX")) {
        sprite->desc.tex2.x = ps_var_f(ss);
    }

    if (ps_var_begin(ss, "tilesY")) {
        sprite->desc.tex2.y = ps_var_f(ss);
    }

    assetPath = context->assetsContext->ParseAssetPath(ss, "renState");
    if (!assetPath.IsEmpty()) {
        if (const auto state = dev->StateLoad(ParseString(ss, "renState"))) {
            sprite->renState = state;
        }
    }

    return true;
}
bool nsSpriteBuilder::SerializeProps(nsScriptSaver &saver, nsVisualObject2d *o, nsVisualCreationContext2d *context) {
    if (!nsVisualBuilder2d::SerializeProps(saver, o, context)) {
        return false;
    }

    const auto sprite = Cast<nsSprite>(o);
    if (!sprite) {
        return false;
    }

    auto &desc = sprite->desc;

    const auto dev = nsRenDevice::Shared()->Device();
    if (desc.tex) {
        context->assetsContext->SaveAssetPath(saver, "texture", dev->TextureGetPath(desc.tex));
    }

    saver.VarFloat2("pivot", desc.center, nsVec2());
    saver.VarFloat2("size", desc.size, nsVec2());
    saver.VarFloat4("color", desc.color, nsColor());

    if (desc.tex1 != nsVec2()) {
        saver.VarFloat2("tex1", desc.tex1, nsVec2());
    }

    saver.VarFloat("tilesX", desc.tex2.x, 1.0f);
    saver.VarFloat("tilesY", desc.tex2.y, 1.0f);

    if (sprite->renState) {
        context->assetsContext->SaveAssetPath(saver, "renState", dev->StateGetPath(sprite->renState));
    }

    return true;
}
