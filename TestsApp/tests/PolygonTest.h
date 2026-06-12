// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file PolygonTest.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "TestCommon.h"
#include "Engine/display/VisualObject2d.h"

class PolygonTest : public BaseFunctionalTest {
public:
    bool Init() override;
    void Loop(float deltaTime) override;
    void Draw() override;
    void Release() override;

private:
    nsVisualObject2d *_sceneRoot = nullptr;
};
