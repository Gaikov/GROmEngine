// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file MultiUserInput.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Core/headers.h"
#include "Engine/UserInput.h"

class nsMultiUserInput : public IUserInput {
public:
    typedef std::vector<IUserInput*> tInputsList;

public:
    virtual ~nsMultiUserInput() = default;
    IPointerTransform   *_pointerTransform = nullptr;

    virtual tInputsList& GetInteractiveItems() {
        return _interactiveItems;
    }

    void ClearInputs();
    void AddInput(IUserInput *input);

protected:
    tInputsList _interactiveItems;

protected:
    void OnKeyUp(int key, int mods) override;

    void OnKeyDown(int key, bool rept, int mods) override;

    void OnChar(char ch) override;

    bool OnPointerUp(float x, float y, int pointerId) override;

    bool OnPointerDown(float x, float y, int pointerId) override;

    bool OnPointerMove(float x, float y, int pointerId) override;

    void OnPointerCancel(int pointerId) override;

    bool OnMouseWheel(float delta) override;

    nsVec2 ScreenToTarget(float x, float y) const;
};