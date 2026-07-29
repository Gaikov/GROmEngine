// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Env.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "DesktopCommon.h"
#include "nsLib/headers.h"

class nsEnv final {
public:
    bool Init();
    GLFWwindow* CreateGameWindow();
    void GetClientSize(int &width, int &height) const;
    void MessagePopup(const char *title, const char *message);
    void OpenUrl(const char *url);
    void MainLoop();
    bool IsMobile();
    void PrepareInput();
    void PrepareDesktopInput();
    void WindowToClientCoordinates(double &x, double &y) const;
    void ClientToWindowCoordinates(double &x, double &y) const;
#ifdef WEB_ASM
    static bool IsKeyPressed(int key);
#endif
    const char* GetDomainName() const {
        return _domainName.c_str();
    }
    void ShowCursor(bool show) const;

    static bool Create();
    static void Destroy();
    static nsEnv* Shared() { return _shared; }

private:
    GLFWwindow *_wnd = nullptr;
    static nsEnv *_shared;
    std::string  _domainName;
};
