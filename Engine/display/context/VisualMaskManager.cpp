//
// Created by Roman on 9/11/2024.
//

#include "VisualMaskManager.h"
#include "RenManager.h"
#include "Core/sys.h"
#include "nsLib/log.h"

nsVisualMaskManager::nsVisualMaskManager() {
    _device = nsRenDevice::Shared()->Device();
    _maskState = _device->StencilLoad("scripts/rs/mask.txt");
    _maskedState = _device->StencilLoad("scripts/rs/masked.txt");
    if (!_maskState || !_maskedState) {
        Sys_FatalError("Stencil state was not loaded!");
    }
}

nsVisualMaskManager::~nsVisualMaskManager() {
    _device->StencilRelease(_maskState);
    _device->StencilRelease(_maskedState);
}

void nsVisualMaskManager::PushMasks(const std::vector<nsVisualMask*> *masks, const nsVisualContext2d &ctx) {
    if (!masks->empty()) {
        _masks.push_back(masks);
        _validBuffer = false;
    }
}

void nsVisualMaskManager::PopMasks(const std::vector<nsVisualMask*> *masks, const nsVisualContext2d &ctx) {
    if (!masks->empty()) {
        auto it = std::remove(_masks.begin(), _masks.end(), masks);
        _masks.erase(it, _masks.end());
        _validBuffer = false;
    }
}

void nsVisualMaskManager::EnsureMasks(const nsVisualContext2d &ctx) {
    if (!_validBuffer) {
        DrawMasks(ctx);
        _validBuffer = true;
    }
}

void nsVisualMaskManager::DrawMasks(const nsVisualContext2d &ctx) {
    if (!_masks.empty()) {
        _drawCount ++;
        _device->StencilApply(nullptr);
        _device->ClearScene(CLR_STENCIL);

        int ref = 0;
        for (auto list : _masks) {
            _maskState->SetStencilRef(ref);
            _device->StencilApply(_maskState);
            for (auto m : (*list)) {
                m->ApplyWorldMatrix();
                m->DrawContent(ctx);
            }
            ref ++;
        }

        _maskedState->SetStencilRef(ref);
        _device->StencilApply(_maskedState);
    } else {
        _device->StencilApply(nullptr);
    }
}

void nsVisualMaskManager::BeginScene() {
    _drawCount = 0;
}

void nsVisualMaskManager::EndScene() {
    _device->StencilApply(nullptr);
    //Log::Info("mask rendered: %i", _drawCount);
}


