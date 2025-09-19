//
// Created by Roman on 9/19/2025.
//

#include "RenderTextureTest.h"

#include "Engine/utils/AppUtils.h"

bool nsRenderTextureTest::Init() {

    _state = _device->StateLoad("default/rs/gui_clamp.txt");

    _rt = _device->RenderTextureCreate(500, 500);
    _rtSprite.tex = _rt;
    _rtSprite.ResetSize();

    _back.tex = _device->TextureLoad("tests/background.jpg", false, TF_RGB);
    _back.ResetSize();
    _back.ComputeCenter();

    _sprite.tex = _device->TextureLoad("tests/coords.png", false);
    _sprite.ResetSize();

    return true;
}

void nsRenderTextureTest::Release() {
    _device->RenderTextureBind(nullptr);
}

void nsRenderTextureTest::Loop(float deltaTime) {
}

void nsRenderTextureTest::Draw() {
    _device->StateApply(_state);

    _back.pos = nsAppUtils::GetClientSize() / 2;
    _back.Draw(_device);

    _sprite.Draw(_device);

    _device->RenderTextureBind(_rt);
    //_device->ClearScene(CLR_ALL);
    _sprite.Draw(_device);
    _device->RenderTextureBind(nullptr);

    _rtSprite.Draw(_device);
}
