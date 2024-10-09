// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file BaseLayout.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "engine/display/VisualContainer2d.h"

class nsBaseLayout : public nsVisualContainer2d {
friend class nsLayoutUpdater;

public:
    enum nsBoundsType {
        MIXED,
        CHILDREN,
        RECTANGLE, //default
    };

    nsProperty<nsBoundsType>    boundsType;
    bool            drawFrame = false;


    nsBaseLayout();

    inline float GetContentWidth() const { return _contentWidth; };
    inline float GetContentHeight() const { return _contentHeight; };

    void SetContentWidth(float value);
    void SetContentHeight(float value);
    void SetWidth(float width) override;
    void SetHeight(float height) override;

    void GetLocalBounds(nsRect &bounds) override;
    void DestroyChildren() override;
    void AddChild(nsVisualObject2d *obj) override;
    void RemoveChild(nsVisualObject2d *obj) override;
    void RemoveChildren() override;

protected:
    const nsEventDispatcher::tEventHandler _onChangeHandler;

    void InvalidateLayout();
    virtual void ComputeChildrenSize();
    virtual void ComputeChildSize(nsVisualObject2d *child) = 0;
    virtual void ComputeChildPlacement(nsVisualObject2d *child) = 0;
    virtual void UpdateChildrenPositions();
    void DrawContent(const nsVisualContext2d &context) override;

private:
    bool _validLayout = true;

    float _contentWidth = 100;
    float _contentHeight = 100;

private:
    void ValidateLayout();
};