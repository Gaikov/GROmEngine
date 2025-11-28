//
// Created by Roman on 8/16/2024.
//

#include "StencilMaskTest.h"
#include "Engine/Platform.h"
#include "Engine/display/VisualSceneRender2d.h"
#include "Engine/KeyCodes.h"
#include "nsLib/log.h"
#include "Engine/SndManager.h"

#define WALL_ANGLE (M_PI / 2)

bool nsStencilMaskTest::Init() {

    _wall.desc.tex = _device->TextureLoad("tests/background.jpg");
    _wall.desc.ResetSize();
    _wall.desc.size *= 1.5;
    _wall.desc.ComputeCenter();
    _wall.origin.angle = WALL_ANGLE;
    _root.AddChild(&_wall);

    _mask.desc.tex = _device->TextureLoad("tests/mask.png");
    _mask.renState = _device->StateLoad("default/rs/gui_alpha_test.txt");
    _mask.desc.size = {600, 600};
    _mask.desc.ComputeCenter().color.FromBytes(0, 255, 0);

    _mask2.desc.size = {400, 400};
    _mask2.desc.ComputeCenter();
    _mask2.desc.color.FromBytes(255,0,100);

    _block.desc.tex = _device->TextureLoad("tests/mask2.png");
    _block.renState = _device->StateLoad("default/rs/gui_alpha_test.txt");
    _block.desc.ResetSize().ComputeCenter().color.FromBytes(200, 255, 0);

    _states.emplace_back([this] {
        _root.AddChild(&_mask);
    });

    _states.emplace_back([this]() -> void {
        _wall.AddMask(&_mask);
    });

    _states.emplace_back([this]() -> void {
        _root.AddChild(&_mask2);
    });

    _states.emplace_back([this]() -> void {
        _wall.AddMask(&_mask2);
    });

    _states.emplace_back([this]() -> void {
        _root.AddChild(&_block);
    });

    _states.emplace_back([this]() -> void {
        _wall.AddMask(&_block);
    });

    _states.emplace_back([this]() -> void {
        _root.RemoveChild(&_mask);
        _root.RemoveChild(&_mask2);
        _root.RemoveChild(&_block);

        _wall.RemoveMask(&_mask);
        _wall.RemoveMask(&_mask2);
        _wall.RemoveMask(&_block);
    });

    auto sd = nsSoundDevice::Shared()->Device();

    auto track = sd->TrackLoad("tests/test.ogg");
    sd->TrackPlay(track);

    return true;
}

void nsStencilMaskTest::Release() {
    nsSoundDevice::Shared()->Device()->TrackPlay(nullptr);
    _root.RemoveChildren();
}

void nsStencilMaskTest::Loop(float deltaTime) {
    _rotationAngle += deltaTime;
    _scale -= deltaTime * 1.5f;
    if (_scale < 1) {
        _scale = 1;
    }

    _root.origin.angle = nsMath::MoveExp(_root.origin.angle, 0, 10, deltaTime);
}

void nsStencilMaskTest::Draw() {
    int w, h;
    App_GetPlatform()->GetClientSize(w, h);

    _root.origin.scale = {_scale, _scale};
    _root.origin.pos = {float(w) / 2, float(h) / 2};

    _mask.origin.angle = _rotationAngle;
    _mask2.origin.pos = {0, 500 * sinf(_rotationAngle * 2)};

    _block.origin.angle = -_rotationAngle * 3;

    nsVisualSceneRender2d::DrawScene(&_root);
}

void nsStencilMaskTest::OnKeyDown(int key, bool rept, int mods) {
    BaseFunctionalTest::OnKeyDown(key, rept, mods);
    if (!rept && key == NS_KEY_SPACE) {
        _states[_currentState]();
        _currentState = (_currentState + 1) % (int)_states.size();

        _scale = 1.5;

        static float angle = nsMath::ToRad(45);
        angle = -angle;
        _root.origin.angle = angle;
    }
}
