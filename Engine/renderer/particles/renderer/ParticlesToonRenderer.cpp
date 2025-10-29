//
// Created by Roman on 10/6/2024.
//

#include "ParticlesToonRenderer.h"
#include "RenManager.h"
#include "Core/ParserUtils.h"

nsParticlesToonRenderer::nsParticlesToonRenderer() :
        _buffer(nsRenDevice::Shared()->Device(), 5000, false) {
}

bool nsParticlesToonRenderer::Parse(script_state_t *ss) {
    auto dev = nsRenDevice::Shared()->Device();

    _front = dev->TextureLoad(ParseString(ss, "frontTexture"));
    _back = dev->TextureLoad(ParseString(ss, "backTexture"));
    _state = dev->StateLoad(ParseString(ss, "renState"));
    ParseColorExt(ss, "borderColor", _borderColor);
    ParseColorExt(ss, "color", _color);
    _borderSize = ParseFloat(ss, "borderSize");

    return true;
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

