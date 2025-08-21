//
// Created by Roman on 11/4/2024.
//

#include "ParticlesEffectHolder.h"

#include "Engine/display/pool/LayoutsPool.h"

nsParticlesEffectHolder::nsParticlesEffectHolder() {
    _active.reserve(50);
    _inactive.reserve(50);
}

nsParticlesEffectHolder::~nsParticlesEffectHolder() {
    RecycleAll();
}

nsParticlesEffect *nsParticlesEffectHolder::CreateEffect(const char *path) {
    const auto pool = nsLayoutsPool::Shared();
    if (const auto visual = pool->Create(path)) {
        if (const auto effect = dynamic_cast<nsParticlesEffect *>(visual)) {
            _active.push_back(effect);
            AddChild(effect);
            return effect;
        }

        Log::Warning("Invalid particles effect class '%s'!", path);
        pool->Recycle(path, visual);
    }
    return nullptr;
}

void nsParticlesEffectHolder::Loop() {
    nsVisualContainer2d::Loop();

    for (auto p : _active) {
        if (!p->IsActive()) {
            _inactive.push_back(p);
        }
    }

    for (auto p : _inactive) {
        RemoveChild(p);
        nsLayoutsPool::Shared()->Recycle(p->GetLayoutPath(), p);

        auto it = std::find(_active.begin(), _active.end(), p);
        assert(it != _active.end());
        _active.erase(it);
    }
    _inactive.clear();
}

void nsParticlesEffectHolder::RecycleAll() {
    for (auto p : _active) {
        RemoveChild(p);
        nsLayoutsPool::Shared()->Recycle(p->GetLayoutPath(), p);
    }
    _active.clear();
    _inactive.clear();
}

bool nsParticlesEffectHolder::HasActiveEffects() {
    for (auto p : _active) {
        if (p->IsActive()) {
            return true;
        }
    }

    return false;
}
