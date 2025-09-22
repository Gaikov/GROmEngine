//
// Created by Roman on 9/9/2024.
//

#include "GLStencilManager.h"

void nsGLStencilManager::Init() {
    _currState = &_default;
    _currState->ForceApply();
}

nsGLStencilState *nsGLStencilManager::AllocateResource(const char *id, int param) {
    return nsGLStencilState::Load(id);
}

void nsGLStencilManager::FreeResource(nsGLStencilState *item) {
    delete item;
}

void nsGLStencilManager::Apply(nsGLStencilState *state) {
    auto prevState = _currState;
    if (!state) {
        state = &_default;
    }

    state->Apply(prevState);
    _currState = state;
}


