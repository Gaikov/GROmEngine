// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file CollisionTest.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/test/BaseFunctionalTest.h"
#include "Engine/display/container/VisualContainer2d.h"
#include "Engine/display/graphics/VisualRect.h"

class nsCollisionTest : public BaseFunctionalTest {
public:
    bool Init() override;
    void Release() override;
    void Loop(float deltaTime) override;
    void Draw() override;

    bool OnPointerMove(float x, float y, int pointerId) override;

private:
    nsVisualContainer2d *_stage = nullptr;
    nsVisualRect        *_rect = nullptr;
    nsVisualRect        *_userRect = nullptr;

    nsVisualRect *CreateRect(float width, float height);
};