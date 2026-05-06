// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file MovieClip.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include <vector>
#include <string>
#include <memory>
#include "Engine/display/VisualObject2d.h"
#include "Engine/renderer/sprites/SpriteDesc.h"
#include "nsLib/models/Property.h"

struct nsAnimClip {
    std::string name;
    float fps = 10;
    std::vector<std::shared_ptr<nsSpriteDesc>> frames;
};

class nsMovieClip : public nsVisualObject2d {
public:
    std::vector<std::shared_ptr<nsAnimClip>> anims;

    nsProperty<std::string> defaultAnim;
    nsProperty<bool>        loop;
    bool      paused   = false;
    IRenState *renState = nullptr;

    nsMovieClip();

    void SetAnim(const char *animName);
    int GetFrame() const { return (int)_currentFrameIdx; }
    int NumFrames() const;

    void AddAnim(std::shared_ptr<nsAnimClip> clip);
    void Stop();
    void GoToFrame(int index);

    const char* GetType() override;
    void GetLocalBounds(nsRect &bounds) override;
    void Loop() override;

protected:
    void DrawContent(const nsVisualContext2d &context) override;

private:
    std::shared_ptr<nsAnimClip>   _currentAnim;
    std::shared_ptr<nsSpriteDesc> _currentFrame;
    float                         _currentFrameIdx = 0;

    const nsEventDispatcher::tEventHandler _onDefaultAnimChanged;

    void UpdateView();
};
