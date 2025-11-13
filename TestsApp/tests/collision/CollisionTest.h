// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file CollisionTest.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/test/BaseFunctionalTest.h"
#include "Engine/display/container/VisualContainer2d.h"
#include "Engine/display/graphics/VisualRect.h"
#include "Engine/display/graphics/VisualCircle.h"

class nsCollisionTest : public BaseFunctionalTest {
public:
    nsCollisionTest();

    bool Init() override;
    void Release() override;
    void Loop(float deltaTime) override;
    void Draw() override;

    bool OnPointerMove(float x, float y, int pointerId) override;
    void OnKeyDown(int key, bool rept) override;
    bool OnMouseWheel(float delta) override;

private:
    nsVisualContainer2d *_stage = nullptr;
    nsVisualRect        *_rect = nullptr;
    nsVisualCircle      *_circle = nullptr;
    nsVisualRect        *_userRect = nullptr;
    nsVisualCircle      *_userCircle = nullptr;

    nsProperty<bool>    _rectColliding;
    float _angle = 0;

    nsVisualRect *CreateRect(float width, float height);
};