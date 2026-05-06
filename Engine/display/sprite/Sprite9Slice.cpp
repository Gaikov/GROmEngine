//
// Created by Roman on 9/16/2024.
//

#include "Sprite9Slice.h"

nsSprite9Slice::nsSprite9Slice()
    : tex(nullptr), width(100), height(100),
      tex1({0, 0}), tex2({0, 0}),
      gridLeft(0), gridRight(0), gridTop(0), gridBottom(0),
      _onChangeHandler([this](const nsBaseEvent *) { _SyncDesc(); }) {
    tex       .AddHandler(nsPropChangedName::CHANGED, _onChangeHandler);
    width     .AddHandler(nsPropChangedName::CHANGED, _onChangeHandler);
    height    .AddHandler(nsPropChangedName::CHANGED, _onChangeHandler);
    tex1      .AddHandler(nsPropChangedName::CHANGED, _onChangeHandler);
    tex2      .AddHandler(nsPropChangedName::CHANGED, _onChangeHandler);
    gridLeft  .AddHandler(nsPropChangedName::CHANGED, _onChangeHandler);
    gridRight .AddHandler(nsPropChangedName::CHANGED, _onChangeHandler);
    gridTop   .AddHandler(nsPropChangedName::CHANGED, _onChangeHandler);
    gridBottom.AddHandler(nsPropChangedName::CHANGED, _onChangeHandler);
}

void nsSprite9Slice::_SyncDesc() {
    _desc.SetTex(tex);
    _desc.SetWidth(width);
    _desc.SetHeight(height);
    _desc.SetAtlasRegion(tex1, tex2);
    _desc.SetGrid(gridLeft, gridRight, gridTop, gridBottom);
}

void nsSprite9Slice::SetGrid(float left, float right, float top, float bottom) {
    gridLeft   = left;
    gridRight  = right;
    gridTop    = top;
    gridBottom = bottom;
}

void nsSprite9Slice::GetLocalBounds(nsRect &bounds) {
    bounds.x      = 0;
    bounds.y      = 0;
    bounds.width  = width;
    bounds.height = height;
}

void nsSprite9Slice::Loop() {
}

void nsSprite9Slice::DrawContent(const nsVisualContext2d &context) {
    _device->StateApply(renState);
    _desc.color = color;
    _desc.Draw(_device);
}

void nsSprite9Slice::SetWidth(float worldWidth) {
    auto s = origin.scale->x;
    width = (s != 0.0f) ? abs(worldWidth / s) : 0.0f;
}

void nsSprite9Slice::SetHeight(float worldHeight) {
    auto s = origin.scale->y;
    height = (s != 0.0f) ? abs(worldHeight / s) : 0.0f;
}
