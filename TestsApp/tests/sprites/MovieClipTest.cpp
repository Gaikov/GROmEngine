// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file MovieClipTest.cpp
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#include "MovieClipTest.h"
#include "Engine/display/VisualSceneRender2d.h"
#include "Engine/display/button/TextButton.h"
#include "Engine/display/layouts/HGroupLayout.h"
#include "Engine/Platform.h"
#include "Engine/KeyCodes.h"
#include "nsLib/log.h"

static constexpr int COLS = 6;
static constexpr int ROWS = 3;

//-----------------------------------------------------------------------------------
// nsMovieClipTest::MakeClip:
//-----------------------------------------------------------------------------------
std::shared_ptr<nsAnimClip> nsMovieClipTest::MakeClip(const char *name, ITexture *tex,
                                                       int row, int cols, int rows) {
    int texW, texH;
    tex->GetSize(texW, texH);

    const float frameW = float(texW) / float(cols);
    const float frameH = float(texH) / float(rows);

    auto clip = std::make_shared<nsAnimClip>();
    clip->name = name;
    clip->fps  = 8.0f;

    for (int col = 0; col < cols; ++col) {
        auto frame   = std::make_shared<nsSpriteDesc>();
        frame->tex   = tex;
        frame->tex1  = nsVec2(float(col)     / float(cols), float(row)     / float(rows));
        frame->tex2  = nsVec2(float(col + 1) / float(cols), float(row + 1) / float(rows));
        frame->size  = nsVec2(frameW, frameH);
        frame->center = nsVec2(frameW * 0.5f, frameH * 0.5f);
        clip->frames.push_back(frame);
    }

    return clip;
}

//-----------------------------------------------------------------------------------
// nsMovieClipTest::CycleAnim:
//-----------------------------------------------------------------------------------
void nsMovieClipTest::CycleAnim() {
    _currentAnim = (_currentAnim + 1) % int(_animNames.size());
    const auto &name = _animNames[_currentAnim];
    _first->SetAnim(name.c_str());
    if (_label) {
        _label->text = ("Anim: " + name).c_str();
    }
}

//-----------------------------------------------------------------------------------
// nsMovieClipTest::Init:
//-----------------------------------------------------------------------------------
bool nsMovieClipTest::Init() {
    auto guiState = _device->StateLoad("default/rs/gui.ggrs");
    auto tex = _device->TextureLoad("tests/anim.jpg", false);
    if (!tex) {
        Log::Error("nsMovieClipTest: failed to load tests/anim.jpg");
        return false;
    }

    _animNames = {"walk", "attack", "idle"};

    // --- top panel: button + label ---
    auto panel = new nsHGroupLayout();
    panel->xMin.SetNumber(10);
    panel->yMax.SetNumber(10);
    _root.AddChild(panel);

    auto btn = new nsTextButton();
    btn->label      = "Next Anim [Space]";
    btn->renState   = guiState;
    btn->SetClickHandler([this]() { CycleAnim(); });
    panel->AddChild(btn);

    _label = new nsTextLabel();
    _label->renState = guiState;
    _label->text     = ("Anim: " + _animNames[0]).c_str();
    panel->AddChild(_label);

    // --- build clips shared between all 3 MovieClips ---
    auto clipWalk   = MakeClip("walk",   tex, 0, COLS, ROWS);
    auto clipAttack = MakeClip("attack", tex, 1, COLS, ROWS);
    auto clipIdle   = MakeClip("idle",   tex, 2, COLS, ROWS);

    // --- place 3 MovieClips horizontally in the center ---
    auto row = new nsHGroupLayout();
    row->gap = 30;
    row->xCenter.SetNumber(0);
    row->yCenter.SetNumber(0);
    _root.AddChild(row);

    const char *defaultAnims[] = {"walk", "attack", "idle"};
    for (int i = 0; i < 3; ++i) {
        auto mc = new nsMovieClip();
        mc->renState = guiState;
        mc->AddAnim(clipWalk);
        mc->AddAnim(clipAttack);
        mc->AddAnim(clipIdle);
        mc->SetAnim(defaultAnims[i]);

        row->AddChild(mc);

        if (i == 0) {
            _first = mc;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------------
// nsMovieClipTest::Release:
//-----------------------------------------------------------------------------------
void nsMovieClipTest::Release() {
    _root.DestroyChildren();
}

//-----------------------------------------------------------------------------------
// nsMovieClipTest::Loop:
//-----------------------------------------------------------------------------------
void nsMovieClipTest::Loop(float deltaTime) {
    int w, h;

    App_GetPlatform()->GetClientSize(w, h);
    _root.Loop();
    _root.SetWidth(float(w));
    _root.SetHeight(float(h));
}

//-----------------------------------------------------------------------------------
// nsMovieClipTest::Draw:
//-----------------------------------------------------------------------------------
void nsMovieClipTest::Draw() {
    nsVisualSceneRender2d::DrawScene(&_root);
}

//-----------------------------------------------------------------------------------
// nsMovieClipTest::OnKeyDown:
//-----------------------------------------------------------------------------------
void nsMovieClipTest::OnKeyDown(int key, bool rept, int mods) {
    BaseFunctionalTest::OnKeyDown(key, rept, mods);
    if (!rept && key == NS_KEY_SPACE) {
        CycleAnim();
    }
}

//-----------------------------------------------------------------------------------
// nsMovieClipTest::GetInput:
//-----------------------------------------------------------------------------------
IUserInput *nsMovieClipTest::GetInput() {
    return &_root;
}
