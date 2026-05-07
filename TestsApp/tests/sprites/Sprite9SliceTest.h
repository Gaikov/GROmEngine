// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Sprite9SliceTest.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/test/BaseFunctionalTest.h"
#include "Engine/display/layouts/GroupLayout.h"
#include "Engine/display/sprite/Sprite9Slice.h"

class nsSprite9SliceTest : public BaseFunctionalTest {
public:
    bool Init() override;
    void Release() override;
    void Loop(float deltaTime) override;
    void Draw() override;

protected:
    IUserInput *GetInput() override;

private:
    nsGroupLayout   _root;

    void CreateFromLayout();
    void CreateFromAtlas();
};
