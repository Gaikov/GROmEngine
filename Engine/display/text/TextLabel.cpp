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
    }
}

void nsTextLabel::DrawContent(const nsVisualContext2d &context) {
    if (!font) {
        return;
    }

    font->Draw(text, nsVec2(0, 0), nsVec2(1, 1), color, text.Length());

    if (drawFrame) {
        nsRect rect;
        GetLocalBounds(rect);
        nsGizmos::DrawRect(rect, nsColor::white);
    }
}

void nsTextLabel::Loop() {

}

void nsTextLabel::SetInt(int value) {
    itoa(value, text.AsChar(), 10);
}
