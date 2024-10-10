// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file VisualMaskManager.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/headers.h"
#include "Engine/display/VisualObject2d.h"

class nsVisualMaskManager {
public:
    nsVisualMaskManager();
    virtual ~nsVisualMaskManager();

    void BeginScene();
    void EndScene();

    void PushMasks(const std::vector<nsVisualMask*> *masks, const nsVisualContext2d &ctx);
    void PopMasks(const std::vector<nsVisualMask*> *masks, const nsVisualContext2d &ctx);

    void EnsureMasks(const nsVisualContext2d &ctx);

private:
    std::vector<const std::vector<nsVisualMask*>*> _masks;
    IRenDevice          *_device;
    IStencilState       *_maskState;
    IStencilState       *_maskedState;
    bool                _validBuffer = true;
    int                 _drawCount = 0;

private:
    void DrawMasks(const nsVisualContext2d &ctx);
};