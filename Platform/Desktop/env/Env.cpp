//
// Created by Roman on 4/16/2025.
//

#include "Env.h"
#include "Engine/engine.h"

static std::vector<int> mouseButtons;
nsEnv *nsEnv::_shared = nullptr;

bool nsEnv::Create() {
    if (!_shared) {
        _shared = new nsEnv();
        return _shared->Init();
    }
    return true;
}

void nsEnv::Destroy() {
    delete _shared;
    _shared = nullptr;
}

void nsEnv::PrepareDesktopInput() {
    glfwSetScrollCallback(_wnd, [](GLFWwindow *window, double x, double y) -> void {
        auto ui = nsEngine::GetActiveInput();
        if (ui) ui->OnMouseWheel((float) y);
    });

    glfwSetCursorPosCallback(_wnd, [](GLFWwindow *window, double xpos, double ypos) -> void {
        if (!mouseButtons.empty()) {
            for (auto button: mouseButtons) {
                nsEngine::OnPointerMove(button, (int) xpos, (int) ypos);
            }
        } else {
            nsEngine::OnPointerMove(-1, (int) xpos, (int) ypos);
        }
    });

    glfwSetMouseButtonCallback(_wnd, [](GLFWwindow *window, int button, int action, int mods) -> void {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        if (action == GLFW_PRESS) {
            mouseButtons.push_back(button);
            nsEngine::OnPointerDown(button, (int) x, (int) y);
        } else if (action == GLFW_RELEASE) {
            auto it = std::remove(mouseButtons.begin(), mouseButtons.end(), button);
            if (it != mouseButtons.end()) {
                mouseButtons.erase(it);
            }
            nsEngine::OnPointerUp(button, (int) x, (int) y);
        }
    });
}
