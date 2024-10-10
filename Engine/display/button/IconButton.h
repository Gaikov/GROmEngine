//
// Created by Roman on 8/28/2024.
//

#pragma once
#include "BaseButton.h"
#include "Engine/renderer/sprites/SpriteDesc.h"

class nsIconButton : public nsBaseButton {
public:
    nsSpriteDesc    icon;
    nsColor         iconColor = nsColor::white;
    nsColor         iconDisabledColor = nsColor::gray;

    ~nsIconButton() override;
    void SetTexture(const char* fileName);

protected:
    ITexture    *_texture = nullptr;

    void DrawContent(const nsVisualContext2d &context) override;
    void GetContentSize(nsVec2 &size) override;
};
