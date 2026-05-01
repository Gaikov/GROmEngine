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

    sprite->desc.Parse(ss, context->assetsContext.get());

    auto assetPath = context->assetsContext->ParseAssetPath(ss, "renState");
    if (!assetPath.IsEmpty()) {
        const auto dev = nsRenDevice::Shared()->Device();
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
