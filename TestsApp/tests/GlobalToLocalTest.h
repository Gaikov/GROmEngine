// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file GlobalToLocalTest.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/display/VisualType.h"
#include "Engine/test/BaseFunctionalTest.h"
#include "Engine/display/container/VisualContainer2d.h"

class nsGlobalToLocalRect : public nsVisualObject2d {
public:
    nsVec2  mousePos;

    void GetLocalBounds(nsRect &bounds) override;
    void DrawContent(const nsVisualContext2d &context) override;
    void Loop() override;
    const char * GetType() override { return nsVisualType::RECTANGLE; }

private:
    float _width = 200;
    float _height = 300;
};

class nsGlobalToLocalTest : public BaseFunctionalTest {
protected:
    bool Init() override;
    void Loop(float deltaTime) override;
    void Draw() override;
    void Release() override;
public:
    bool OnPointerMove(float x, float y, int pointerId) override;
private:
    nsVisualContainer2d _container;
    nsGlobalToLocalRect _rect;
};