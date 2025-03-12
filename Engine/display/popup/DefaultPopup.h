// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file DefaultPopup.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "BasePopup.h"

class nsDefaultPopup : public nsBasePopup {
protected:
    nsTextLabel *_caption;
    float _captionHeight = 0;
    float _contentPaddings = 10;
    nsSprite *_captionBack;

    nsDefaultPopup();
    void ComputeChildrenSize() override;
private:
    bool OnCreate() override;

};