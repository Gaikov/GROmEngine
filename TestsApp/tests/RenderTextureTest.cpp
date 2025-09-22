//
// Created by Roman on 9/19/2025.
//

#include "RenderTextureTest.h"

#include "Engine/TimeFormat.h"
#include "Engine/utils/AppUtils.h"

#define BUFFER_WIDTH 100
#define BUFFER_HEIGHT 100

bool nsRenderTextureTest::Init() {

    _state = _device->StateLoad("default/rs/gui_clamp.txt");

    _rt = _device->RenderTextureCreate(BUFFER_WIDTH, BUFFER_HEIGHT);
    _rtSprite.tex = _rt;

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
    nsMatrix m;
    const auto clientSize = nsAppUtils::GetClientSize();

    _device->StateApply(_state);

    _back.pos = clientSize / 2;
    _back.Draw(_device);

    _device->RenderTextureBind(_rt);


    m.Identity();
    m.SetOrthoRH(0, clientSize.x,  clientSize.y, 0, -1, 1);
    _device->LoadProjMatrix(m);

    _device->ClearScene(CLR_ALL);

    _sprite.color = {1, 1, 1, 0.9f};
    _sprite.size = clientSize;
    _sprite.Draw(_device);

    _sprite.color = nsColor::white;
    _sprite.ResetSize();
    _sprite.Draw(_device);
    _device->RenderTextureBind(nullptr);

    static float angle = 0;
    angle += g_frameTime * 10;

    m.SetOrthoDimRH(clientSize.x, clientSize.y);
    _device->LoadProjMatrix(m);
    _rtSprite.size = clientSize;
    _rtSprite.Draw(_device);
}
