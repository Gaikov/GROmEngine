// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file TextLabel.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "engine/display/VisualObject2d.h"
#include "engine/Font.h"
#include "nsLib/color.h"

class nsTextLabel : public nsVisualObject2d {
public:
    nsFont      *font = nullptr;
    nsString    text;
    nsColor     color;
    bool        drawFrame = false;

    void SetInt(int value);

    nsTextLabel();
    void Loop() override;

    void GetLocalBounds(nsRect &bounds) override;
    void DrawContent(const nsVisualContext2d &context) override;
};