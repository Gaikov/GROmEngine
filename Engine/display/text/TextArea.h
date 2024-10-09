// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file TextArea.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "engine/display/VisualObject2d.h"
#include "engine/display/align/Align.h"
#include "nsLib/color.h"

class nsTextArea : public nsVisualObject2d {
public:
    nsColor         color;
    bool            drawFrame = false;
    nsAlign::Type   hAlign = nsAlign::BEGIN;

    nsTextArea();
    void SetLineSpace(float space);
    void SetFont(nsFont *font);
    void SetText(const char *text);

    void GetLocalBounds(nsRect &bounds) override;
    void Loop() override;
protected:
    void DrawContent(const nsVisualContext2d &context) override;

private:
    struct Line {
        nsString    text;
        nsRect      bounds;
    };

    nsFont              *_font;
    float               _lineSpace = 0;
    std::vector<Line>   _lines;

    bool                _validMetrics = false;
    nsRect              _bounds;

    void ValidateMetrics();
};