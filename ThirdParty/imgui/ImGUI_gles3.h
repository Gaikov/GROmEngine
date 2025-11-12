// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ImGUI_gles3.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/UserInput.h"

class nsImGUI_gles3 : public IUserInput {
public:
    virtual ~nsImGUI_gles3() = default;
    virtual bool Init(void *window);
    virtual void Shutdown();

    virtual void StartFrame();
    virtual void EndFrame();

    bool OnPointerUp(float x, float y, int pointerId) override;
    bool OnPointerDown(float x, float y, int pointerId) override;
    bool OnPointerMove(float x, float y, int pointerId) override;
    void OnPointerCancel(int pointerId) override;
    void OnKeyUp(int key) override;
    void OnKeyDown(int key, bool rept) override;
    void OnChar(char ch) override;
    void OnMouseWheel(float delta) override;
};
