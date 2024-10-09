//
// Created by Roman on 8/28/2024.
//

#include "IconButton.h"

nsIconButton::~nsIconButton() {
    _device->TextureRelease(_texture);
}

void nsIconButton::SetTexture(const char *fileName) {
    _device->TextureRelease(_texture);
    icon.tex = _texture = nullptr;

    if (StrCheck(fileName)) {
        icon.tex = _texture = _device->TextureLoad(fileName);
        icon.ResetSize();
    }
}

void nsIconButton::DrawContent(const nsVisualContext2d &context) {
    nsBaseButton::DrawContent(context);

    icon.color = IsEnabled() ? iconColor : iconDisabledColor;
    icon.center = {0, 0};
    icon.pos = {paddings, paddings};
    icon.Draw(_device);
}

void nsIconButton::GetContentSize(nsVec2 &size) {
    size = icon.size;
}
