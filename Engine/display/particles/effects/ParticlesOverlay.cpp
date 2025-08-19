//
// Created by Roman on 11/11/2024.
//

#include "ParticlesOverlay.h"

bool nsParticlesOverlay::OnInit() {
    nsSubSystem::OnInit();

    _layer = new nsParticlesEffectHolder();
    return true;
}

void nsParticlesOverlay::OnRelease() {
    auto parent = _layer->GetParent();
    if (parent) {
        parent->RemoveChild(_layer);
    }

    _layer->RecycleAll();
    _layer->Destroy();
    _layer = nullptr;

    nsSubSystem::OnRelease();
}

nsParticlesEffect *nsParticlesOverlay::Create(const char *path) {
    return _layer->CreateEffect(path);
}

void nsParticlesOverlay::CreateBlast(const char *path, const nsVec2 &pos) {
    auto effect = _layer->CreateEffect(path);
    effect->origin.pos = pos;
    effect->Emit(false);
    effect->Blast();
}
