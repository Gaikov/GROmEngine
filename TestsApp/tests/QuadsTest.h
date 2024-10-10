// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file QuadsTest.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/test/BaseFunctionalTest.h"
#include "Engine/renderer/QuadsBuffer.h"
#include "Engine/RenDevice.h"

class nsQuadsTest : public BaseFunctionalTest {
public:
    bool Init() override;

    void Loop(float deltaTime) override;

    void Draw() override;

    void Release() override;

private:
    nsQuadsBuffer *_quads;
    ITexture *_tex;
};