//
// Created by Roman on 6/26/2024.
//

#include "TextButton.h"
#include "Engine/RenManager.h"
#include "renderer/font/FontsCache.h"

nsTextButton::nsTextButton() {
    font = nsFontsCache::Shared()->SysFont();
}

void nsTextButton::DrawContent(const nsVisualContext2d &context) {
    nsBaseButton::DrawContent(context);

    nsColor textColor = IsEnabled() ? labelColor : labelDisabledColor;

    if (font) {
        font->Draw(label, nsVec2(paddings, paddings), labelScale, textColor);
    }
}

void nsTextButton::GetContentSize(nsVec2 &size) {
    if (font) {
        font->GetSize(label, size, label.Length());
        size *= labelScale;
    }
}