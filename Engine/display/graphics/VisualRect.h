// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file VisualRect.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/display/VisualObject2d.h"
#include "nsLib/color.h"

class nsVisualRect : public nsVisualObject2d {
public:
    nsRect  rect;
    nsColor color;

    void GetLocalBounds(nsRect &bounds) override;
    void Loop() override;
    void DrawContent(const nsVisualContext2d &context) override;
};