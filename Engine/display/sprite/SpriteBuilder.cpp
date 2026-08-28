//
// Created by Roman on 10/14/2024.
//

#include "SpriteBuilder.h"
#include "Sprite.h"
#include "RenManager.h"
#include "Core/ParserUtils.h"

nsVisualObject2d *nsSpriteBuilder::CreateDefault(nsVisualCreationContext2d *) {
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
        if (const auto state = dev->StateLoad(assetPath)) {
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

    sprite->desc.Save(saver, context->assetsContext.get());

    if (sprite->renState) {
        const auto dev = nsRenDevice::Shared()->Device();
        context->assetsContext->SaveAssetPath(saver, "renState", dev->StateGetPath(sprite->renState));
    }

    return true;
}
