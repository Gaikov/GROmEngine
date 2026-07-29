// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file BaseGame2DApp.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/GameApp.h"
#include "Engine/display/container/VisualContainer2d.h"
#include "Engine/input/MultiUserInput.h"
#include "nsLib/math/ortho/OrthogonalNativeView.h"

struct IRenDevice;

/**
 * Base application for a 2D stage rendered and queried in one coordinate system.
 * Derived lifecycle methods should call the corresponding base implementation.
 */
class nsBaseGame2DApp : public IGameApp {
public:
    bool Init() override;
    void Release() override;
    void DrawWorld() override;
    void Loop(float frameTime) override;

    IUserInput *GetUserInput() override;
    void GetGUIDimension(int &width, int &height) override;

protected:
    nsVisualContainer2d *GetStage() const { return _stage; }
    IRenDevice *GetRenderDevice() const { return _device; }
    nsOrthogonalView *GetOrthogonalView() const { return _orthogonalView; }

    /**
     * Uses a non-owned view for both rendering and pointer transformation.
     * Pass nullptr to restore the built-in native view.
     */
    void SetOrthogonalView(nsOrthogonalView *view);
    void DrawStage();

private:
    void SyncOrthogonalScreenSize();

private:
    nsVisualContainer2d *_stage = nullptr;
    IRenDevice *_device = nullptr;

    nsMultiUserInput _input;
    nsOrthogonalNativeView _nativeView;
    nsOrthogonalView *_orthogonalView = &_nativeView;

    int _screenWidth = 1;
    int _screenHeight = 1;
};
