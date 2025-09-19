// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file RenderTextureTest.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/renderer/sprites/SpriteDesc.h"
#include "Engine/test/BaseFunctionalTest.h"

class nsRenderTextureTest final : public BaseFunctionalTest {
public:
    bool Init() override;
    void Release() override;
    void Loop(float deltaTime) override;
    void Draw() override;

private:
    IRenderTexture *_rt = nullptr;
    nsSpriteDesc _back;
    nsSpriteDesc _sprite;
};
