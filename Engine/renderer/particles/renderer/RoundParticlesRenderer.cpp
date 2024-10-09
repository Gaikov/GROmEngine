//
// Created by Roman on 9/25/2024.
//
#include "RoundParticlesRenderer.h"
#include "Core/ParserUtils.h"
#include "RenManager.h"
#include "nsLib/StrTools.h"

nsParticlesRoundRenderer::nsParticlesRoundRenderer() {
    _buffer = new nsQuadsBuffer(nsRenDevice::Shared()->Device(), 1000, true);
}

nsParticlesRoundRenderer::~nsParticlesRoundRenderer() {
    delete _buffer;
}

void nsParticlesRoundRenderer::Draw(nsParticle *head) {

    auto device = nsRenDevice::Shared()->Device();

    device->TextureBind(texture);
    device->StateApply(renState);

    auto p = head;
    while (p) {
        float size = p->size;
        float pivot = size / 2;

        if (_buffer->AddQuad(
                p->pos,
                {pivot, pivot},
                {size, size},
                p->color, {0, 0}, {1, 1},
                p->angle)) {
            p = p->next;
        } else {
            _buffer->Draw();
            _buffer->Clear();
        }
    }

    _buffer->Draw();
    _buffer->Clear();
}

bool nsParticlesRoundRenderer::Parse(script_state_t *ss) {
    const auto dev = nsRenDevice::Shared()->Device();
    texture = dev->TextureLoad(ParseStrP(ss, "texture"));
    renState = dev->StateLoad(nsString(ParseStrP(ss, "renState")));
    return true;
}

