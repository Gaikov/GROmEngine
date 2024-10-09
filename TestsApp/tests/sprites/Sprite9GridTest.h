// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Sprite9GridTest.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "engine/test/BaseFunctionalTest.h"
#include "engine/renderer/sprites/Sprite9Grid.h"

class nsSprite9GridTest : public BaseFunctionalTest {
public:
    bool Init() override;

    void Loop(float deltaTime) override;

    void Draw() override;

    void Release() override;
private:
    nsSprite9Grid _sprite;
};