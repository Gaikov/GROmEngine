// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file BasePopup.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/display/layouts/GroupLayout.h"
#include "Engine/display/text/TextLabel.h"
#include "Engine/display/Sprite.h"

class nsBasePopup : public nsGroupLayout {
    friend class nsPopupManager;

protected:
    nsTextLabel *_caption;
    float _captionHeight = 0;
    float _contentPaddings = 10;
    nsSprite *_captionBack;
    nsVisualObject2d *_content = nullptr;

    nsBasePopup();

    virtual nsVisualObject2d* CreateContent() = 0;
    virtual void DestroyContent(nsVisualObject2d *content) = 0;
    void ComputeChildrenSize() override;

private:
    bool OnCreate();
    void OnDestroy();

};