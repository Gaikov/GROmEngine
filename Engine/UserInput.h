// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file UserInput.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/Vec2.h"

struct IPointerTransform {
    virtual nsVec2 ScreenToTarget(float x, float y) = 0;
};

enum nsPointerType {
    POINTER_MOUSE,
    POINTER_TOUCH,
    POINTER_CONTROLLER,
    POINTER_UNKNOWN
};

struct nsPointerData {
    nsPointerType type = POINTER_UNKNOWN;
    nsVec2 pos;
    int pointerId = -1;
};

struct IPointersInput {
    virtual bool OnPointerUp(const nsPointerData &pointer) = 0;

    virtual bool OnPointerDown(const nsPointerData &pointer) = 0;

    virtual bool OnPointerMove(const nsPointerData &pointer) = 0;

    virtual void OnPointerCancel(const nsPointerData &pointer) = 0;
};

struct IKeyboardInput {
    virtual void OnKeyUp(int key, int mods) = 0;

    virtual void OnKeyDown(int key, bool rept, int mods) = 0;

    virtual void OnChar(char ch) = 0;
};

struct IUserInput : public IPointersInput, public IKeyboardInput {
    virtual bool OnMouseWheel(float delta) = 0;
};
