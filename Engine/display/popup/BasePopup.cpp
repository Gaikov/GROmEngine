//
// Created by Roman on 8/13/2024.
//

#include "BasePopup.h"

#include "PopupManager.h"

nsBasePopup::nsBasePopup() {
    float pad = 5;
    _caption = new nsTextLabel();
    _caption->text = "Popup Test";
    _caption->origin.scale = {0.5, 1};
    _caption->xCenter.SetNumber(0);
    _caption->yMax.SetNumber(pad);
    _captionHeight = _caption->GetHeight() + pad * 2;

    auto cb = _captionBack = new nsSprite();
    cb->desc.size = {100, _captionHeight};
    cb->desc.color = nsColor::gray;
    cb->xMin.SetNumber(0);
    cb->xMax.SetNumber(0);
    cb->yMax.SetNumber(0);
    nsBaseLayout::AddChild(_captionBack);
    nsBaseLayout::AddChild(_caption);

    cb = new nsSprite();
    cb->desc.color = nsColor::darkGray;
    cb->xMin.SetNumber(0);
    cb->yMin.SetNumber(0);
    cb->xMax.SetNumber(0);
    cb->yMax.SetNumber(_captionHeight);
    nsBaseLayout::AddChild(cb);

    xCenter.SetNumber(0);
    yCenter.SetNumber(0);

    SetContentWidth(200);
    SetContentHeight(200);
}

bool nsBasePopup::OnCreate() {
    _content = CreateContent();
    if (!_content) {
        return false;
    }

    _content->xCenter.SetNumber(0);
    _content->yMin.SetNumber(10);
    AddChild(_content);
    return true;
}

void nsBasePopup::OnDestroy() {
    RemoveChild(_content);
    DestroyContent(_content);
}

void nsBasePopup::ComputeChildrenSize() {
    nsBaseLayout::ComputeChildrenSize();

    nsRect  rect;
    _content->GetBounds(rect, this);

    SetContentWidth(rect.width + _contentPaddings * 2);
    SetContentHeight( rect.height + _contentPaddings * 2 + _captionHeight);
}


