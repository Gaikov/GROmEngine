//
// Created by Roman on 4/16/2025.
//

#include "DesktopEnv.h"
#include <windows.h>
#include "Core/sys.h"
#include "Engine/engine.h"
#include "DesktopCommon.h"

bool nsDesktopEnv::Init() {
    return true;
}

void nsDesktopEnv::MessagePopup(const char *title, const char *message) {
    ::MessageBox(nullptr, message, title, 0);
}

void nsDesktopEnv::OpenUrl(const char *url) {
    ShellExecute(nullptr, "open", url, nullptr, nullptr, SW_SHOWNORMAL);
}

void nsDesktopEnv::MainLoop(GLFWwindow *wnd) {
    while (!glfwWindowShouldClose(wnd) && !Sys_IsExit()) {
        nsEngine::MainLoop();
        glfwPollEvents();
    }
}
