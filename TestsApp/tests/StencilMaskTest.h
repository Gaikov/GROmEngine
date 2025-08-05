// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file StencilMaskTest.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/test/BaseFunctionalTest.h"
#include "Engine/display/sprite/Sprite.h"
#include "Engine/display/container/VisualContainer2d.h"
#include "Engine/display/context/VisualMaskManager.h"

class nsStencilMaskTest : public BaseFunctionalTest {
public:
    bool Init() override;
    void Release() override;
    void Loop(float deltaTime) override;
    void Draw() override;
    void OnKeyDown(int key, bool rept) override;

private:
    nsSprite    _mask;
    nsSprite    _mask2;
    float       _rotationAngle = 0;

    nsSprite    _wall;
    nsSprite    _block;
    nsVisualContainer2d _root;
    float       _scale = 1;

    std::vector<std::function<void()>>  _states;
    int                                 _currentState = 0;
};