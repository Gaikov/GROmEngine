//
// Created by Roman on 10/15/2024.
//

#include "VisualParticles.h"
#include "TimeFormat.h"
#include "display/container/VisualContainer2d.h"


void nsVisualParticles::GetLocalBounds(nsRect &bounds) {
    bounds = {0, 0, 1, 1};
    //TODO: calculate bounds
}

void nsVisualParticles::Loop() {
    if (space == GLOBAL) {
        _system.MoveTo(origin.ToGlobal({}));

        auto up = origin.GetWorld().TransformVector({0, 1});
        _system.RotateTo(up.GetAngle());
    } else if (space == PARENT) {
        _system.MoveTo(origin.pos);
        _system.RotateTo(origin.angle);
    }

    _system.Update(g_frameTime);
}

void nsVisualParticles::DrawContent(const nsVisualContext2d &context) {
    _system.Draw();
}

void nsVisualParticles::ApplyWorldMatrix() {
    if (space == GLOBAL) {
        _device->LoadMatrix(nsMatrix::identity);
    } else if (space == PARENT) {
        if (GetParent()) {
            nsMatrix    m;
            GetParent()->origin.GetWorld().ToMatrix3(m);
            _device->LoadMatrix(m);
        } else {
            _device->LoadMatrix(nsMatrix::identity);
        }
    } else {
        nsVisualObject2d::ApplyWorldMatrix();
    }
}

void nsVisualParticles::ResetPosition() {
    if (space == GLOBAL) {
        _system.SetPosition(origin.ToGlobal({}));

        auto up = origin.GetWorld().TransformVector({0, 1});
        _system.SetRotation(up.GetAngle());
    } else if (space == PARENT) {
        _system.MoveTo(origin.pos);
        _system.RotateTo(origin.angle);
    }
}
