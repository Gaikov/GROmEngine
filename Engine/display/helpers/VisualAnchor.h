// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file VisualAnchor.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "display/VisualObject2d.h"
#include "nsLib/color.h"

class nsVisualAnchor : public nsVisualObject2d {
public:
    nsVisualAnchor();

    float   size = 10;
    nsColor color = nsColor::red;
    IRenState *renState = nullptr;

    void GetLocalBounds(nsRect &bounds) override;
    void Loop() override;
    void DrawContent(const nsVisualContext2d &context) override;
};