// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Sprite9SliceTest.cpp
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#include "Sprite9SliceTest.h"
#include "Engine/display/VisualSceneRender2d.h"
#include "Engine/display/factory/VisualFactory2d.h"
#include "Engine/display/text/TextLabel.h"
#include "Engine/display/layouts/VGroupLayout.h"
#include "Engine/Platform.h"
#include "nsLib/log.h"

//-----------------------------------------------------------------------------------
// Helper: small caption label
//-----------------------------------------------------------------------------------
static nsTextLabel *MakeLabel(const char *text, IRenDevice *dev) {
    auto label = new nsTextLabel();
    label->renState = dev->StateLoad("default/rs/gui.ggrs");
    label->text = text;
    return label;
}

//-----------------------------------------------------------------------------------
// nsSprite9SliceTest::CreateFromLayout:
// Loads sprite9.layout — container with two children:
//   "standalone" — frame9grid.png (whole texture, no atlas region)
//   "from_atlas" — atlas00.png with explicit tex1/tex2 pixel region
// Both are added to the root as-is to verify builder output.
//-----------------------------------------------------------------------------------
void nsSprite9SliceTest::CreateFromLayout() {
    auto col = new nsVGroupLayout();
    col->gap = 20;
    col->xMin.SetNumber(20);
    col->yCenter.SetNumber(0);
    _root.AddChild(col);

    col->AddChild(MakeLabel("From layout file:", _device));

    auto obj = nsVisualFactory2d::Shared()->Create("tests/layouts/sprite9.layout");
    if (!obj) {
        Log::Error("Sprite9SliceTest: failed to load tests/layouts/sprite9.layout");
        return;
    }
    // obj is a container with "standalone" and "from_atlas" as children
    col->AddChild(obj);
}

//-----------------------------------------------------------------------------------
// nsSprite9SliceTest::CreateFromAtlas:
// Creates Res_WindowBackNew 9-slice programmatically to cross-check
// against the layout file version.
//-----------------------------------------------------------------------------------
void nsSprite9SliceTest::CreateFromAtlas() {
    auto col = new nsVGroupLayout();
    col->gap = 20;
    col->xMax.SetNumber(20);
    col->yCenter.SetNumber(0);
    _root.AddChild(col);

    col->AddChild(MakeLabel("From code (Res_WindowBackNew):", _device));

    auto tex = _device->TextureLoad("tests/atlas00.png", false);
    if (!tex) {
        Log::Error("Sprite9SliceTest: failed to load tests/atlas00.png");
        return;
    }

    // Res_WindowBackNew: x=715 y=1275 w=322 h=285
    // gridLeft=87 gridRight=80 gridTop=93 gridBottom=127
    const float sizes[][2] = {{322, 285}, {500, 400}, {200, 150}};
    for (auto &sz : sizes) {
        auto slice = new nsSprite9Slice();
        slice->renState   = _device->StateLoad("default/rs/gui.ggrs");
        slice->tex        = tex;
        slice->tex1       = nsVec2(715, 1275);
        slice->tex2       = nsVec2(1037, 1560);
        slice->gridLeft   = 87;
        slice->gridRight  = 80;
        slice->gridTop    = 93;
        slice->gridBottom = 127;
        slice->SetWidth(sz[0]);
        slice->SetHeight(sz[1]);
        col->AddChild(slice);
    }
}

//-----------------------------------------------------------------------------------
// nsSprite9SliceTest::Init:
//-----------------------------------------------------------------------------------
bool nsSprite9SliceTest::Init() {
    CreateFromLayout();
    CreateFromAtlas();
    return true;
}

//-----------------------------------------------------------------------------------
// nsSprite9SliceTest::Release:
//-----------------------------------------------------------------------------------
void nsSprite9SliceTest::Release() {
    _root.DestroyChildren();
}

//-----------------------------------------------------------------------------------
// nsSprite9SliceTest::Loop:
//-----------------------------------------------------------------------------------
void nsSprite9SliceTest::Loop(float deltaTime) {
    int w, h;
    App_GetPlatform()->GetClientSize(w, h);
    _root.SetWidth(float(w));
    _root.SetHeight(float(h));
}

//-----------------------------------------------------------------------------------
// nsSprite9SliceTest::Draw:
//-----------------------------------------------------------------------------------
void nsSprite9SliceTest::Draw() {
    nsVisualSceneRender2d::DrawScene(&_root);
}

//-----------------------------------------------------------------------------------
// nsSprite9SliceTest::GetInput:
//-----------------------------------------------------------------------------------
IUserInput *nsSprite9SliceTest::GetInput() {
    return &_root;
}
