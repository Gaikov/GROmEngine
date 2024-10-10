// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file VisualObject2d.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/math/Rect.h"
#include "nsLib/math/Transform2.h"
#include "Engine/RenDevice.h"
#include "VisualContext2d.h"
#include "Engine/display/anchors/LayoutAnchor.h"

class nsVisualContainer2d;

class nsVisualMask;

class nsVisualObject2d {
    friend class nsVisualContainer2d;

public:
    typedef std::vector<nsVisualObject2d *> list_t;

public:
    nsTransform2    origin;
    bool            visible = true;

    nsLayoutAnchor  xCenter, yCenter;
    nsLayoutAnchor  xMin, xMax, yMin, yMax;
    IRenDevice* const _device;

public:
    nsVisualObject2d();
    virtual ~nsVisualObject2d();
    virtual void Destroy();

    virtual float GetWidth();
    virtual void SetWidth(float width);
    virtual float GetHeight();
    virtual void SetHeight(float height);

    virtual bool HitTest(float x, float y);
    virtual void GetLocalBounds(nsRect &bounds) = 0;
    virtual void GetBounds(nsRect &bounds, nsVisualObject2d *targetSpace);

    virtual void Loop() = 0;

    void AddMask(nsVisualMask *mask);
    void RemoveMask(nsVisualMask *mask);

    virtual void DrawNode(const nsVisualContext2d &context);
    virtual void ApplyWorldMatrix();
    virtual void DrawContent(const nsVisualContext2d &context) = 0;

    nsVisualContainer2d *GetParent() const;

    static void LeaksCheck();

private:
    nsVisualContainer2d *_parent = nullptr;
    std::vector<nsVisualMask*> _masks;

    static int _leakCounter;
};

class nsVisualMask : public nsVisualObject2d {

};