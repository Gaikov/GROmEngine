//
// Created by Roman on 8/11/2024.
//

#include "PopupManager.h"
#include "display/Sprite.h"
#include "nsLib/log.h"

class nsPopupOverlay final : public nsSprite, public IUserInput {
public:
    explicit nsPopupOverlay(IRenState *state) {
        renState = state;
        desc.size = {100, 100};
        desc.color = nsColor(0, 0, 0, 0.5);
        xMin.SetNumber(0);
        xMax.SetNumber(0);
        yMin.SetNumber(0);
        yMax.SetNumber(0);
    }

    bool OnPointerUp(float x, float y, int pointerId) override {
        return true;
    }

    bool OnPointerDown(float x, float y, int pointerId) override {
        return true;
    }

    bool OnPointerMove(float x, float y, int pointerId) override {
        return true;
    }

    void OnPointerCancel(int pointerId) override {
    }

    void OnKeyUp(int key) override {}
    void OnKeyDown(int key, bool rept) override {}
    void OnChar(char ch) override {}
    void OnMouseWheel(float delta) override {}
};

nsPopupManager::nsPopupManager() {
    _root.xMax.SetNumber(0);
    _root.yMax.SetNumber(0);
    _root.xMin.SetNumber(0);
    _root.yMin.SetNumber(0);
}

void nsPopupManager::OnRelease() {
    Log::Info("...destroying popup manager");
    const auto parent = _root.GetParent();
    if (parent) {
        parent->RemoveChild(&_root);
    }
    _root.DestroyChildren();
    _popupsList.clear();
    nsSubSystem::OnRelease();
}

void nsPopupManager::AddPopup(nsBasePopup *popup) {
    const auto overlay = new nsPopupOverlay(overlayState);

    _root.AddChild(overlay);
    _root.AddChild(popup);

    _popupsList.push_back({overlay, popup});
}

void nsPopupManager::DestroyPopup(nsBasePopup *popup) {
    auto it = std::find_if(_popupsList.begin(), _popupsList.end(), [&popup](const PopupInfo &info) -> bool {
        return info.popup == popup;
    });

    if (it != _popupsList.end()) {
        it->popup->OnDestroy();

        _root.RemoveChild(it->popup);
        it->popup->Destroy();
        _root.RemoveChild(it->overlay);
        it->overlay->Destroy();

        _popupsList.erase(it);
    }
}


