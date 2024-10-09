// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ScrollBar.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "engine/display/layouts/BaseLayout.h"

class nsScrollBar : public nsBaseLayout {
public:
    nsProperty<float>   thumbPaddings;
    nsProperty<float>   contentSize;
    nsProperty<float>   pageSize;
    nsProperty<float>   scrollPosition;

    nsScrollBar();
    ~nsScrollBar() override;

    bool IsAllowed() const { return pageSize < contentSize; }
    void SetBackground(nsVisualObject2d *back);
    void SetThumb(nsVisualObject2d *thumb);
    void DestroyChildren() override;

protected:
    bool _down = false;
    int _pointerId = -1;
    float _offset = 0;

    nsVisualObject2d *_back = nullptr;
    nsVisualObject2d *_thumb = nullptr;
    nsEventDispatcher::tEventHandler _onScrollChanged;

    void ComputeChildSize(nsVisualObject2d *child) override;
    void ComputeChildPlacement(nsVisualObject2d *child) override;
    float ComputeThumbSize();
    float ComputeThumbPos();
    float ComputeScrollPos(float localPos);

    bool OnPointerDown(float x, float y, int pointerId) override;
    bool OnPointerMove(float x, float y, int pointerId) override;
    void OnPointerCancel(int pointerId) override;
    bool OnPointerUp(float x, float y, int pointerId) override;
};