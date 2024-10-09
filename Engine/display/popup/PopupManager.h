// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file PopupManager.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/SubSystem.h"
#include "engine/display/layouts/GroupLayout.h"
#include "nsLib/color.h"
#include "BasePopup.h"

class nsPopupManager : public nsSubSystem<nsPopupManager> {
public:
    nsColor overlayColor = nsColor(nsColor::gray, 0.5);
    IRenState   *overlayState = nullptr;

    nsPopupManager();

    template<typename TPopupClass>
    TPopupClass* CreatePopup() {
        auto    popup = new TPopupClass();
        if (popup->OnCreate()) {
            AddPopup(popup);
            return popup;
        }
        popup->Destroy();
        return nullptr;
    }

    nsVisualObject2d* GetRoot() { return &_root; }

    void DestroyPopup(nsBasePopup *popup);

private:
    struct PopupInfo {
        nsVisualObject2d *overlay;
        nsBasePopup *popup;
    };

    nsGroupLayout           _root;
    std::vector<PopupInfo>  _popupsList;

    void AddPopup(nsBasePopup *popup);
    void OnRelease() override;
};