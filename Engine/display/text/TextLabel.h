// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file TextLabel.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/display/VisualObject2d.h"
#include "Engine/Font.h"
#include "nsLib/color.h"
#include "Engine/display/align/Align.h"

class nsTextLabel : public nsVisualObject2d {
public:
    nsFont          *font = nullptr;
    IRenState       *renState = nullptr;
    nsString        text;
    nsColor         color;
    bool            drawFrame = false;
    nsAlign::Type   hAlign = nsAlign::BEGIN;
    float           letterSpace = 0;

    void SetInt(int value);

    nsTextLabel();
    void Loop() override;

    void GetLocalBounds(nsRect &bounds) override;
    void DrawContent(const nsVisualContext2d &context) override;
};