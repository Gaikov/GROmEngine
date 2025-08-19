//
// Created by Roman on 11/4/2024.
//

#include "ParticlesEffect.h"

void nsParticlesEffect::OnVisualCreated(const char *path) {
    _path = path;
}

bool nsParticlesEffect::ParseCustomProps(script_state_t *ss) {
    _particles.clear();

    IterateRecursive([this](nsVisualObject2d *child) {
        auto p = dynamic_cast<nsVisualParticles *>(child);
        if (p) {
            _particles.push_back(p);
        }
        return true;
    });

    return true;
}

void nsParticlesEffect::OnVisualPrepare() {
}

void nsParticlesEffect::OnVisualRecycled() {
}

void nsParticlesEffect::Emit(bool value) {
    for (auto p: _particles) {
        p->GetSystem().spawnEnabled = value;
    }
}

bool nsParticlesEffect::IsActive() {
    for (auto p: _particles) {
        if (p->GetSystem().IsActive()) {
            return true;
        }
    }
    return false;
}

void nsParticlesEffect::Blast() {
    for (auto p: _particles) {
        p->ResetPosition();
        p->GetSystem().Emit();
    }
}

void nsParticlesEffect::SetPosition(const nsVec2 &pos) {
    origin.pos = pos;
    for (auto p: _particles) {
        p->ResetPosition();
    }
}

void nsParticlesEffect::Reset(bool emit) {
    for (auto p : _particles) {
        p->GetSystem().Reset(emit);
    }
}

