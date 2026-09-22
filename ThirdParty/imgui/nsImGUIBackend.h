// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
#pragma once

#include <memory>

#include "Engine/UserInput.h"
#include "imgui.h"

class nsImGUIBackend : public IUserInput {
public:
    virtual ~nsImGUIBackend() = default;

    bool Init(void *window);
    void Shutdown();

    void StartFrame();
    void EndFrame();

    bool OnPointerUp(float x, float y, int pointerId) override;
    bool OnPointerDown(float x, float y, int pointerId) override;
    bool OnPointerMove(float x, float y, int pointerId) override;
    void OnPointerCancel(int pointerId) override;
    void OnKeyUp(int key, int mods) override;
    void OnKeyDown(int key, bool repeat, int mods) override;
    void OnChar(char ch) override;
    bool OnMouseWheel(float delta) override;

protected:
    virtual bool BackendInit(void *window) = 0;
    virtual void BackendShutdown() = 0;
    virtual void BackendStartFrame() = 0;
    virtual void BackendRenderDrawData(ImDrawData *drawData) = 0;

private:
    void ShowDockSpace();

private:
    bool _initialized = false;
};

std::unique_ptr<nsImGUIBackend> CreateImGUIBackend();
