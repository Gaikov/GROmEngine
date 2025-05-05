//
// Created by Roman on 4/16/2025.
//

#include <windows.h>
#include "Core/sys.h"
#include "Engine/engine.h"
#include "DesktopCommon.h"
#include "env/Env.h"

bool nsEnv::Init() {
    return true;
}

void nsEnv::MessagePopup(const char *title, const char *message) {
    ::MessageBox(nullptr, message, title, 0);
}

void nsEnv::OpenUrl(const char *url) {
    ShellExecute(nullptr, "open", url, nullptr, nullptr, SW_SHOWNORMAL);
}

void nsEnv::MainLoop() {
    while (!glfwWindowShouldClose(_wnd) && !Sys_IsExit()) {
        nsEngine::MainLoop();
        glfwPollEvents();
    }
}

void nsEnv::GetClientSize(int &width, int &height) {
    glfwGetWindowSize(_wnd, &width, &height);
}

GLFWwindow* nsEnv::CreateGameWindow() {
    _wnd = glfwCreateWindow(800, 600, "GROm Engine", nullptr, nullptr);
    return _wnd;
}

bool nsEnv::IsMobile() {
    return false;
}

