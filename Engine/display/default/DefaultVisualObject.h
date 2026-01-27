// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file DefaultVisualObject.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/display/VisualObject2d.h"
#include "renderer/sprites/SpriteDesc.h"

class nsDefaultVisualObject : public nsVisualObject2d {
public:
    nsDefaultVisualObject();
    void SetTagName(const char *tag);
    const char * GetType() override;
    void GetLocalBounds(nsRect &bounds) override;
    void Loop() override;
    void DrawContent(const nsVisualContext2d &context) override;

private:
    std::string _tagName;
    nsSpriteDesc _sprite;
    IRenState *_renState = nullptr;
};
