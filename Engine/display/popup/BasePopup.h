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
    nsVisualObject2d *_content = nullptr;

    virtual nsVisualObject2d* CreateContent() = 0;
    virtual void DestroyContent(nsVisualObject2d *content) = 0;

    virtual bool OnCreate();
    virtual void OnDestroy();
};