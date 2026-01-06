//
// Created by Roman on 10/6/2024.
//

#include "ParticlesToonRenderer.h"
#include "RenManager.h"
#include "Core/ParserUtils.h"
#include "assets/VisualAssetsContext.h"

nsParticlesToonRenderer::nsParticlesToonRenderer() :
        _buffer(nsRenDevice::Shared()->Device(), 5000, false) {
    _name = NAME;
}

bool nsParticlesToonRenderer::Parse(script_state_t *ss, const nsVisualAssetsContext *context) {
    auto dev = nsRenDevice::Shared()->Device();

    frontTexture = dev->TextureLoad(context->ParseAssetPath(ss, "frontTexture"));
    backTexture = dev->TextureLoad(context->ParseAssetPath(ss, "backTexture"));
    renState = dev->StateLoad(context->ParseAssetPath(ss, "renState"));
    ParseColorExt(ss, "borderColor", backColor);
    ParseColorExt(ss, "color", frontColor);
    borderSize = ParseFloat(ss, "borderSize");

    return true;
}

void nsParticlesToonRenderer::Save(const nsScriptSaver &saver, const nsVisualAssetsContext *context) {
    const auto dev = nsRenDevice::Shared()->Device();

    saver.VarString("frontTexture", context->RelativeAssetPath(dev->TextureGetPath(frontTexture)));
    saver.VarString("backTexture", context->RelativeAssetPath(dev->TextureGetPath(backTexture)));
    saver.VarString("renState", context->RelativeAssetPath(dev->StateGetPath(renState)));
    saver.VarFloat4("borderColor", backColor, nsColor());
    saver.VarFloat4("color", frontColor, nsColor());
    saver.VarFloat("borderSize", borderSize, 0);
}

void nsParticlesToonRenderer::Draw(nsParticle *head) {
    auto device = nsRenDevice::Shared()->Device();

    device->StateApply(renState);

    device->TextureBind(backTexture);
    device->SetColor(backColor);
    DrawPass(head, borderSize * 2);

    device->TextureBind(frontTexture);
    device->SetColor(frontColor);
    DrawPass(head, 0);
}

void nsParticlesToonRenderer::DrawPass(nsParticle *head, float addSize) {
    _buffer.Clear();

    auto p = head;
    while (p) {
        float size = p->size + addSize;
        float pivot = size / 2;

        if (_buffer.AddQuad(
                p->pos,
                {pivot, pivot},
                {size, size},
                p->color, {0, 0}, {1, 1},
                p->angle)) {
            p = p->next;
        } else {
            _buffer.Draw();
            _buffer.Clear();
        }
    }

    _buffer.Draw();
}

