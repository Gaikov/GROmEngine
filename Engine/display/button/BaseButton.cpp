//
// Created by Roman on 8/28/2024.
//

#include "BaseButton.h"
#include "Engine/RenAux.h"
#include "Engine/renderer/sprites/SpriteDesc.h"
#include "Engine/RenManager.h"
#include "nsLib/log.h"

void nsBaseButton::SetClickHandler(const handler_t &handler) {
    _handler = handler;
}

void nsBaseButton::DrawContent(const nsVisualContext2d &context) {
    nsRect r;
    GetLocalBounds(r);

    nsColor c = colorUp;
    if (IsEnabled()) {
        if (IsPointerDown()) {
            c = colorDown;
        } else if (IsPointerOver()) {
            c = colorOver;
        }
    }

    nsSpriteDesc desc;
    desc.size.x = r.width;
    desc.size.y = r.height;
    desc.color = c;
    desc.Draw(g_renDev);

    if (drawFrame) {
        RX_DrawRect(r.x, r.y, r.EndX(), r.EndY(), frameColor);
    }
}

void nsBaseButton::OnClick() {
    if (_handler) {
        _handler();
    }
}

void nsBaseButton::GetLocalBounds(nsRect &r) {
    nsVec2 size;
    GetContentSize(size);

    r.x = 0;
    r.width = std::max(size.x + paddings * 2, minWidth);

    r.y = 0;
    r.height = paddings * 2 + size.y;
}

void nsBaseButton::Loop() {

}

bool nsBaseButton::HitTest(float x, float y) {
    return nsVisualObject2d::HitTest(x, y);
}