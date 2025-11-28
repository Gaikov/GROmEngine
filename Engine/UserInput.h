// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file UserInput.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef    _UserInput_H_
#define    _UserInput_H_

#include "nsLib/Vec2.h"

struct IPointerTransform {
    virtual nsVec2 ScreenToTarget(float x, float y) = 0;
};

struct IPointersInput {
    virtual bool OnPointerUp(float x, float y, int pointerId) = 0;

    virtual bool OnPointerDown(float x, float y, int pointerId) = 0;

    virtual bool OnPointerMove(float x, float y, int pointerId) = 0;

    virtual void OnPointerCancel(int pointerId) = 0;
};

struct IKeyboardInput {
    virtual void OnKeyUp(int key, int mods) = 0;

    virtual void OnKeyDown(int key, bool rept, int mods) = 0;

    virtual void OnChar(char ch) = 0;
};

struct IUserInput : public IPointersInput, public IKeyboardInput {
    virtual bool OnMouseWheel(float delta) = 0;
};

#endif //_UserInput_H_