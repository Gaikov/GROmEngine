// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file InteractiveElement.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/UserInput.h"

class nsInteractivePolicy {
public:
    virtual ~nsInteractivePolicy() = default;

    virtual void OnPointerOver() = 0;
    virtual void OnPointerOut() = 0;

    virtual void OnPointerDown() = 0;
    virtual void OnPointerUp() = 0;
    virtual void OnClick() = 0;
};

class   nsInteractiveElement : virtual public IUserInput, virtual public nsInteractivePolicy {
public:
    virtual void SetEnabled(bool enabled);
    virtual bool IsEnabled() const;

    bool IsPointerOver() const;
    bool IsPointerDown() const;

    nsInteractiveElement();

    virtual bool HitTest(float x, float y) = 0;

    void AddPolicy(nsInteractivePolicy *p);

protected:
    void OnKeyUp(int key, int mods) override;

    void OnKeyDown(int key, bool rept, int mods) override;

    void OnChar(char ch) override;

    bool OnPointerUp(const nsPointerData &pointer) override;

    bool OnPointerDown(const nsPointerData &pointer) override;

    bool OnPointerMove(const nsPointerData &pointer) override;

    void OnPointerCancel(const nsPointerData &pointer) override;

    bool OnMouseWheel(float delta) override;

private:
    bool _pointerDown = false;
    int _pointerDownId = -1;
    bool _pointerOver = false;
    int _pointerOverId = -1;

    bool _enabled = true;

    std::vector<nsInteractivePolicy*>   _policies;
};