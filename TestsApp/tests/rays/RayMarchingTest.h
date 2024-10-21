// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file RayMarchingTest.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/test/BaseFunctionalTest.h"
#include "Engine/display/container/VisualContainer2d.h"
#include "RayBoxTest.h"
#include "RayLightTest.h"
#include "Engine/display/Sprite.h"

class nsRayMarchingTest : public BaseFunctionalTest {
public:
    bool Init() override;
    void Loop(float deltaTime) override;
    void Draw() override;
    void Release() override;
    bool OnPointerMove(float x, float y, int pointerId) override;

private:
    nsVec2 _mousePos;
    nsVisualContainer2d _root;
    std::vector<nsRayBoxTest*>  _boxes;
    std::vector<nsRayLightTest*> _lights;
    std::vector<nsSprite*>  _lamps;
    nsVisualContainer2d *_lampsLayer = nullptr;

    nsSprite    *_floor = nullptr;

    float _angle = 0;

private:
    void CreateBox(const nsVec2 &pos, float width, float height, float angleSpeed);
    void CreateLight(ITexture *tex, float radius, nsColor color);
};