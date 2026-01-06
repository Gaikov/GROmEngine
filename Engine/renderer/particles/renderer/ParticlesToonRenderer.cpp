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

    _front = dev->TextureLoad(context->ParseAssetPath(ss, "frontTexture"));
    _back = dev->TextureLoad(context->ParseAssetPath(ss, "backTexture"));
    _state = dev->StateLoad(context->ParseAssetPath(ss, "renState"));
    ParseColorExt(ss, "borderColor", _borderColor);
    ParseColorExt(ss, "color", _color);
    _borderSize = ParseFloat(ss, "borderSize");

    return true;
}

void nsParticlesToonRenderer::Save(const nsScriptSaver &saver, const nsVisualAssetsContext *context) {
    const auto dev = nsRenDevice::Shared()->Device();

    saver.VarString("frontTexture", context->RelativeAssetPath(dev->TextureGetPath(_front)));
    saver.VarString("backTexture", context->RelativeAssetPath(dev->TextureGetPath(_back)));
    saver.VarString("renState", context->RelativeAssetPath(dev->StateGetPath(_state)));
    saver.VarFloat4("borderColor", _borderColor, nsColor());
    saver.VarFloat4("color", _color, nsColor());
    saver.VarFloat("borderSize", _borderSize, 0);
}

void nsParticlesToonRenderer::Draw(nsParticle *head) {
    auto device = nsRenDevice::Shared()->Device();

    device->StateApply(_state);

    device->TextureBind(_back);
    device->SetColor(_borderColor);
    DrawPass(head, _borderSize * 2);

    device->TextureBind(_front);
    device->SetColor(_color);
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

