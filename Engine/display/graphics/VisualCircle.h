// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file VisualCircle.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/display/VisualObject2d.h"
#include "Engine/display/VisualType.h"
#include "nsLib/color.h"

class nsVisualCircle : public nsVisualObject2d {
public:
    float radius = 100;
    nsColor color;


    void GetLocalBounds(nsRect &bounds) override;
    const char * GetType() override { return nsVisualType::CIRCLE; }
    void Loop() override;
    void DrawContent(const nsVisualContext2d &context) override;
};