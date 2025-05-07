// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file TextArea.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/display/VisualObject2d.h"
#include "Engine/display/align/Align.h"
#include "nsLib/color.h"

class nsTextArea : public nsVisualObject2d {
public:
    nsColor         color;
    bool            drawFrame = false;
    nsAlign::Type   hAlign = nsAlign::BEGIN;
    IRenState       *renState = nullptr;

    nsTextArea();
    void SetLineSpace(float space);
    float GetLineSpace() const { return _lineSpace; }
    void SetFont(nsFont *font);
    void SetText(const char *text);
    const char *GetText() const { return _text.c_str(); }

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
    std::string         _text;

    bool                _validMetrics = false;
    nsRect              _bounds;

    void ValidateMetrics();
};