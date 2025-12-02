// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file RayLightTest.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/display/VisualObject2d.h"
#include "nsLib/color.h"
#include "RayBoxTest.h"

class nsRayLightTest : public nsVisualObject2d {
public:
    const int NUM_RAYS = 360;
    nsColor color;

    nsRayLightTest(ITexture *tex, float radius);

    const char* GetType() override { return "RayLightTest"; }
    void CastShadows(const std::vector<nsRayBoxTest*> &objects);

    void GetLocalBounds(nsRect &bounds) override;
    void Loop() override;

protected:
    float _radius;

    std::vector<nsVec2> _rays;
    std::vector<nsVec2> _castedPoints;

    static IVertexBuffer   *_vb;
    static IRenState        *_rs;
    ITexture        *_tex;

    void DrawContent(const nsVisualContext2d &context) override;

private:
    void EnsureBuffer();
};