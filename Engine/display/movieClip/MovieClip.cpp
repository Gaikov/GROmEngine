// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file MovieClip.cpp
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#include "MovieClip.h"
#include "RenAux.h"
#include "Engine/display/VisualType.h"
#include "Engine/TimeFormat.h"

nsMovieClip::nsMovieClip() {
}

void nsMovieClip::AddAnim(const nsAnimClip &anim) {
    _anims.push_back(anim);
    if (!_currentAnim) {
        _currentAnim = &_anims.back();
        updateView();
    }
}

void nsMovieClip::SetAnim(const char *animName) {
    for (auto &a : _anims) {
        if (a.name == animName) {
            _currentAnim = &a;
            _currentFrameIdx = 0;
            updateView();
            return;
        }
    }
}

int nsMovieClip::NumFrames() const {
    return _currentAnim ? (int)_currentAnim->frames.size() : 0;
}

void nsMovieClip::Stop() {
    _currentFrameIdx = (float)(int)_currentFrameIdx;
}

void nsMovieClip::GoToFrame(int index) {
    if (_currentAnim && index >= 0 && index < (int)_currentAnim->frames.size()) {
        _currentFrameIdx = (float)index;
        updateView();
    }
}

void nsMovieClip::Loop() {
    if (paused || !_currentAnim || _currentAnim->frames.empty()) return;

    float prevFrame = _currentFrameIdx;
    _currentFrameIdx += _currentAnim->fps * g_frameTime;

    int numFrames = (int)_currentAnim->frames.size();
    if (_currentFrameIdx >= numFrames) {
        if (loop) {
            _currentFrameIdx = fmodf(_currentFrameIdx, (float)numFrames);
        } else {
            _currentFrameIdx = (float)(numFrames - 1);
            paused = true;
        }
    }

    if ((int)_currentFrameIdx != (int)prevFrame) {
        updateView();
    }
}

void nsMovieClip::updateView() {
    if (!_currentAnim || _currentAnim->frames.empty()) {
        _currentFrame = nullptr;
        return;
    }

    int index = (int)_currentFrameIdx;
    if (index >= (int)_currentAnim->frames.size()) index = 0;

    _currentFrame = &_currentAnim->frames[index];
}

void nsMovieClip::GetLocalBounds(nsRect &bounds) {
    if (_currentFrame) {
        bounds.x = -_currentFrame->center.x;
        bounds.y = -_currentFrame->center.y;
        bounds.width = _currentFrame->size.x;
        bounds.height = _currentFrame->size.y;
    } else {
        bounds = nsRect(0, 0, 0, 0);
    }
}

void nsMovieClip::DrawContent(const nsVisualContext2d &context) {
    if (_currentFrame) {
        if (renState) {
            _device->StateApply(renState);
        }
        _currentFrame->Draw(_device);
    }
}

const char* nsMovieClip::GetType() {
    return nsVisualType::MOVIE_CLIP;
}
