// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file BaseGame2DApp.cpp
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------

#include "BaseGame2DApp.h"

#include "Engine/Platform.h"
#include "Engine/RenManager.h"
#include "Engine/display/VisualSceneRender2d.h"

bool nsBaseGame2DApp::Init() {
    if (_stage) {
        return false;
    }

    _device = nsRenDevice::Shared()->Device();
    if (!_device) {
        return false;
    }

    _stage = nsVisualContainer2d::CreateStage();
    if (!_stage) {
        _device = nullptr;
        return false;
    }

    _input.ClearInputs();
    _input.AddInput(_stage);
    _input._pointerTransform = _orthogonalView;
    SyncOrthogonalScreenSize();
    return true;
}

void nsBaseGame2DApp::Release() {
    _input._pointerTransform = nullptr;
    _input.ClearInputs();

    if (_stage) {
        _stage->Destroy();
        _stage = nullptr;
    }

    _device = nullptr;
    _orthogonalView = &_nativeView;
}

void nsBaseGame2DApp::DrawWorld() {
    SyncOrthogonalScreenSize();
    _device->LoadProjMatrix(_orthogonalView->GetViewMatrix());
    _device->LoadViewMartix(nsMatrix::identity);
    _device->LoadMatrix(nsMatrix::identity);
    DrawStage();
}

void nsBaseGame2DApp::Loop(float) {
    if (_stage) {
        _stage->Loop();
    }
}

IUserInput *nsBaseGame2DApp::GetUserInput() {
    SyncOrthogonalScreenSize();
    return &_input;
}

void nsBaseGame2DApp::GetGUIDimension(int &width, int &height) {
    SyncOrthogonalScreenSize();
    width = _screenWidth;
    height = _screenHeight;
}

void nsBaseGame2DApp::SetOrthogonalView(nsOrthogonalView *view) {
    _orthogonalView = view ? view : &_nativeView;
    _input._pointerTransform = _orthogonalView;
}

void nsBaseGame2DApp::DrawStage() {
    if (_stage) {
        nsVisualSceneRender2d::DrawScene(_stage);
    }
}

void nsBaseGame2DApp::SyncOrthogonalScreenSize() {
    int width;
    int height;
    App_GetPlatform()->GetClientSize(width, height);
    if (width <= 0 || height <= 0) {
        return;
    }

    _screenWidth = width;
    _screenHeight = height;
    _orthogonalView->SetScreenSize(static_cast<float>(width), static_cast<float>(height));
}
