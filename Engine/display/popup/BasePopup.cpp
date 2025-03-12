//
// Created by Roman on 8/13/2024.
//

#include "BasePopup.h"

#include "PopupManager.h"

bool nsBasePopup::OnCreate() {
    _content = CreateContent();
    if (!_content) {
        return false;
    }

    AddChild(_content);
    return true;
}

void nsBasePopup::OnDestroy() {
    RemoveChild(_content);
    DestroyContent(_content);
}

