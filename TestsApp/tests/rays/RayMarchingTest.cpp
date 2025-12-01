//
// Created by Roman on 8/2/2024.
//

#include "RayMarchingTest.h"
#include "RayBoxTest.h"
#include "Engine/display/VisualSceneRender2d.h"
#include "Engine/Platform.h"
#include "Engine/RenManager.h"
#include "Engine/renderer/sprites/SpriteDesc.h"


bool nsRayMarchingTest::Init() {
    _lampsLayer = new nsVisualContainer2d();

    auto dev = nsRenDevice::Shared()->Device();
    auto t = dev->TextureLoad("tests/dlight.jpg", false, TF_RGB);

    CreateLight(t, 300, nsColor(1, 1, 0.8, 0.8));
    CreateLight(t, 500, nsColor(0, 1, 1, 0.8));
    CreateLight(t, 500, nsColor(0, 1, 0, 0.8));

    _floor = new nsSprite();
    _floor->renState = dev->StateLoad("default/rs/lm.txt");
    _floor->desc.tex = dev->TextureLoad("tests/background.jpg");
    _root.AddChild(_floor);

    CreateBox(nsVec2(), 200, 50, 45);
    CreateBox(nsVec2(0, 200), 50, 50, -10);
    CreateBox(nsVec2(0, -200), 50, 50, 10);

    float startY = -450;
    float height = 900;
    int numBoxes = 10;
    for (int i = 0; i < numBoxes; i++) {
        float y = height / float(numBoxes - 1) * i + startY;
        CreateBox(nsVec2(-200, y), 30, 30, 0);
        CreateBox(nsVec2(200, y), 30, 30, 0);
    }

    _root.AddChild(_lampsLayer);

    return true;
}

bool nsRayMarchingTest::OnPointerMove(float x, float y, int pointerId) {
    BaseFunctionalTest::OnPointerMove(x, y, pointerId);

    int winWidth, winHeight;
    App_GetPlatform()->GetClientSize(winWidth, winHeight);
    _mousePos.x = x;
    _mousePos.y = float(winHeight) - y;
    return true;
}

void nsRayMarchingTest::Release() {
    _root.DestroyChildren();
}

void nsRayMarchingTest::Loop(float deltaTime) {
    _root.Loop();
    _angle += deltaTime;
}

void nsRayMarchingTest::Draw() {
    int w,h;

    App_GetPlatform()->GetClientSize(w, h);
    nsVec2  size = nsVec2(float(w), float(h));
    _root.origin.pos = size / 2;

    _floor->desc.tex->GetSize(w, h);
    nsSpriteDesc &desc = _floor->desc;
    desc.size = size;
    desc.tex2 = nsVec2(size.x / w, size.y / h) / 2;
    desc.center = size / 2;

    _lights[0]->origin.pos = _root.origin.ToLocal(_mousePos);
    _lights[1]->origin.pos = nsVec2(-300, size.y / 2 * sinf(_angle));
    _lights[2]->origin.pos = nsVec2(300, -size.y / 2 * sinf(_angle));

    for (int i = 0; i < _lights.size(); i++) {
        auto l = _lights[i];
        l->CastShadows(_boxes);
        auto lamp = _lamps[i];
        lamp->origin.pos = nsVec2(l->origin.pos);
    }

    nsVisualContext2d   ctx;
    ctx.updateLayouts = false;
    nsVisualSceneRender2d::DrawScene(&_root);
}

void nsRayMarchingTest::CreateBox(const nsVec2 &pos, float width, float height, float angleSpeed) {
    auto box = new nsRayBoxTest(width, height);
    box->rotationSpeed = nsMath::ToRad(angleSpeed);
    box->origin.pos = pos;
    _root.AddChild(box);
    _boxes.push_back(box);
}

void nsRayMarchingTest::CreateLight(ITexture *tex, float radius, nsColor color) {
    auto light = new nsRayLightTest(tex, radius);
    light->color = color;
    _root.AddChild(light);
    _lights.push_back(light);

    auto dev = nsRenDevice::Shared()->Device();
    auto lamp = new nsSprite();
    lamp->desc.color = nsColor(color, 1);
    lamp->desc.tex = dev->TextureLoad("default/gizmos/lamp.png");
    lamp->renState = dev->StateLoad("default/rs/gui.ggrs");
    lamp->desc.ResetSize().ComputeCenter();
    _lamps.push_back(lamp);
    _lampsLayer->AddChild(lamp);
}


