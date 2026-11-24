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
    nsMultiUserInput() { _interactiveItems.reserve(4); }
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

    bool OnPointerUp(const nsPointerData &pointer) override;

    bool OnPointerDown(const nsPointerData &pointer) override;

    bool OnPointerMove(const nsPointerData &pointer) override;

    void OnPointerCancel(const nsPointerData &pointer) override;

    bool OnMouseWheel(float delta) override;

private:
    bool DispatchPointer(const nsPointerData &pointer, bool (IPointersInput::*handler)(const nsPointerData &));
};
