//
// Created by Roman on 11/11/2024.
//

#include "ParticlesOverlay.h"
#include "display/pool/LayoutsPool.h"

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

void nsParticlesOverlay::CreateBlast(const uint32_t effectId, const nsVec2 &pos) {
    const auto it =_effectsMap.find(effectId);
    if (it != _effectsMap.end()) {
        CreateBlast(it->second.c_str(), pos);
    } else {
        Log::Warning("Effect ID is not registered: %i", effectId);
    }
}

void nsParticlesOverlay::RegisterEffect(const uint32_t effectId, const std::string &path, const int reserved) {
    _effectsMap[effectId] = path;
    nsLayoutsPool::Shared()->Reserve(path.c_str(), reserved);
}

void nsParticlesOverlay::RecycleAllEffects() {
    _layer->RecycleAll();
}
