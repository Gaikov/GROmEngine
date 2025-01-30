//
// Created by Roman on 7/13/2024.
//

#include "TextLabel.h"
#include "Engine/RenManager.h"
#include "RenAux.h"
#include "renderer/font/FontsCache.h"

nsTextLabel::nsTextLabel() {
    font = nsFontsCache::Shared()->SysFont();
}

void nsTextLabel::GetLocalBounds(nsRect &bounds) {
    if (font) {
        font->GetBounds(text, bounds);
        bounds.width += (letterSpace * (float)text.Length() - 1);
        if (hAlign == nsAlign::CENTER) {
            bounds.x -= bounds.width / 2;
        } else if (hAlign == nsAlign::END) {
            bounds.x -= bounds.width;
        }
    }
}

void nsTextLabel::DrawContent(const nsVisualContext2d &context) {
    if (!font) {
        return;
    }

    nsRect rect;
    GetLocalBounds(rect);

    _device->StateApply(renState);
    font->Draw(text, nsVec2(rect.x, rect.y), nsVec2(1, 1), color, text.Length(), 0, letterSpace);

    if (drawFrame) {
        nsGizmos::DrawRect(rect, nsColor::white);
    }
}

void nsTextLabel::Loop() {

}

void nsTextLabel::SetInt(int value) {
    text.Format("%i", value);
}
