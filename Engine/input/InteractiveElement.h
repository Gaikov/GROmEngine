// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file InteractiveElement.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/UserInput.h"

class   nsInteractiveElement : public IUserInput {
public:
    virtual void SetEnabled(bool enabled);
    virtual bool IsEnabled() const;

    bool IsPointerOver() const;
    bool IsPointerDown() const;

    virtual ~nsInteractiveElement() = default;

    virtual bool HitTest(float x, float y) = 0;

protected:
    void OnKeyUp(int key) override;

    void OnKeyDown(int key, bool rept) override;

    void OnChar(char ch) override;

    bool OnPointerUp(float x, float y, int pointerId) override;

    bool OnPointerDown(float x, float y, int pointerId) override;

    bool OnPointerMove(float x, float y, int pointerId) override;

    void OnPointerCancel(int pointerId) override;
protected:

    void OnMouseWheel(float delta) override;

    virtual void OnPointerOver() = 0;
    virtual void OnPointerOut() = 0;

    virtual void OnPointerDown() = 0;
    virtual void OnPointerUp() = 0;
    virtual void OnClick() = 0;

private:
    bool _pointerDown = false;
    bool _pointerOver = false;
    int _pointerId = -1;
    bool _enabled = true;
};