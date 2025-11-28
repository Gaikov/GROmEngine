// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file nsEngineContext.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "RenDevice.h"
#include "nsLib/math/ortho/OrthogonalNativeView.h"
#include "display/layouts/GroupLayout.h"
#include "nsLib/SubSystem.h"
#include "input/MultiUserInput.h"

class nsEngineContext : public nsSubSystem<nsEngineContext>, IKeyboardInput {
public:
    nsGroupLayout* GetUIRoot() { return _root; }
    IRenState* GetUIState() { return _rsGUI; }
    nsOrthogonalNativeView& GetUIOrtho() { return _nativeOrtho; }
    IUserInput* GetActiveInput();
    nsVisualContext2d& GetContext2D() { return _context2d; }

protected:
    bool OnInit() override;
    void OnRelease() override;

private:
    void OnKeyUp(int key, int mods) override;
    void OnKeyDown(int key, bool rept, int mods) override;
    void OnChar(char ch) override;

private:
    nsVisualContext2d       _context2d;
    IRenState	            *_rsGUI = nullptr;
    nsOrthogonalNativeView  _nativeOrtho;
    nsGroupLayout           *_root = nullptr;
    nsMultiUserInput        _engineInput;
    nsMultiUserInput        _input;
};