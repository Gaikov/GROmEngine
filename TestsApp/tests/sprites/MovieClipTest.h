// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file MovieClipTest.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/test/BaseFunctionalTest.h"
#include "Engine/display/layouts/GroupLayout.h"
#include "Engine/display/movieClip/MovieClip.h"
#include "Engine/display/text/TextLabel.h"

class nsMovieClipTest : public BaseFunctionalTest {
public:
    bool Init() override;
    void Release() override;
    void Loop(float deltaTime) override;
    void Draw() override;
    void OnKeyDown(int key, bool rept, int mods) override;
    IUserInput *GetInput() override;

private:
    nsGroupLayout   _root;
    nsMovieClip    *_first  = nullptr;
    nsTextLabel    *_label  = nullptr;

    std::vector<std::string> _animNames;
    int _currentAnim = 0;

    std::shared_ptr<nsAnimClip> MakeClip(const char *name, ITexture *tex,
                                         int row, int cols, int rows);
    void CycleAnim();
};
