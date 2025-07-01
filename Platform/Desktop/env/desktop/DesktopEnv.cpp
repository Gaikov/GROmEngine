//
// Created by Roman on 4/16/2025.
//

#include "Core/sys.h"
#include "Engine/engine.h"
#include "DesktopCommon.h"
#include "env/Env.h"

bool nsEnv::Init() {
    return true;
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

void nsEnv::PrepareInput() {
    PrepareDesktopInput();
}

void nsEnv::ShowCursor(bool show) const {
    glfwSetInputMode(_wnd,
                     GLFW_CURSOR,
                     show ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
}