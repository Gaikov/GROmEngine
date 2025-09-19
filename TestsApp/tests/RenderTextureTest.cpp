//
// Created by Roman on 9/19/2025.
//

#include "RenderTextureTest.h"

#include "Engine/utils/AppUtils.h"

bool nsRenderTextureTest::Init() {
    _rt = _device->RenderTextureCreate(100, 100);
    _back.tex = _device->TextureLoad("tests/background.jpg", false, TF_RGB);

    _back.ResetSize();
    _back.ComputeCenter();

    _back.tex = _rt;

    return true;
}

void nsRenderTextureTest::Release() {
    _device->RenderTextureBind(nullptr);
}

void nsRenderTextureTest::Loop(float deltaTime) {
}

void nsRenderTextureTest::Draw() {
    _back.pos = nsAppUtils::GetClientSize() / 2;
    _back.Draw(_device);
}
