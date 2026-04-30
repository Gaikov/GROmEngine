// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file MovieClip.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include <vector>
#include <string>
#include "Engine/display/VisualObject2d.h"
#include "Engine/renderer/sprites/SpriteDesc.h"

struct nsAnimClip {
    std::string                 name;
    float                       fps = 10;
    std::vector<nsSpriteDesc>   frames;
};

class nsMovieClip : public nsVisualObject2d {
public:
    bool    paused = false;
    bool    loop = true;
    IRenState *renState = nullptr;

    nsMovieClip();

    void SetAnim(const char *animName);
    int GetFrame() const { return (int)_currentFrameIdx; }
    int NumFrames() const;

    void AddAnim(const nsAnimClip &anim);
    void Stop();
    void GoToFrame(int index);

    const char* GetType() override;
    void GetLocalBounds(nsRect &bounds) override;
    void Loop() override;

protected:
    void DrawContent(const nsVisualContext2d &context) override;

private:
    nsAnimClip              *_currentAnim = nullptr;
    nsSpriteDesc            *_currentFrame = nullptr;
    std::vector<nsAnimClip> _anims;
    float                   _currentFrameIdx = 0;

    void updateView();
};
