// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file RayBoxTest.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/display/VisualObject2d.h"
#include "RayMarching.h"

class nsRayBoxTest : public nsVisualObject2d {
public:
    float rotationSpeed = 0;
    nsBoxFunction const sdFunc;

    nsRayBoxTest(float width, float height);


    void GetLocalBounds(nsRect &bounds) override;
    void Loop() override;

protected:
    void DrawContent(const nsVisualContext2d &context) override;

private:
    float _width, _height;
};