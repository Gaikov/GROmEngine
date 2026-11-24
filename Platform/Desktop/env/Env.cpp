//
// Created by Roman on 4/16/2025.
//

#include "Env.h"
#include "Engine/engine.h"

static std::vector<int> mouseButtons;
nsEnv *nsEnv::_shared = nullptr;

static void GetPointerScale(GLFWwindow *window, double &scaleX, double &scaleY) {
#ifdef __EMSCRIPTEN__
    float contentScaleX;
    float contentScaleY;
    glfwGetWindowContentScale(window, &contentScaleX, &contentScaleY);
    scaleX = contentScaleX;
    scaleY = contentScaleY;
#else
    int windowWidth, windowHeight;
    int framebufferWidth, framebufferHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    scaleX = windowWidth > 0 && framebufferWidth > 0 ? double(framebufferWidth) / windowWidth : 1.0;
    scaleY = windowHeight > 0 && framebufferHeight > 0 ? double(framebufferHeight) / windowHeight : 1.0;
#endif
}

void nsEnv::WindowToClientCoordinates(double &x, double &y) const {
    double scaleX, scaleY;
    GetPointerScale(_wnd, scaleX, scaleY);
    x *= scaleX;
    y *= scaleY;
}

void nsEnv::ClientToWindowCoordinates(double &x, double &y) const {
    double scaleX, scaleY;
    GetPointerScale(_wnd, scaleX, scaleY);
    x /= scaleX;
    y /= scaleY;
}

bool nsEnv::Create() {
    if (!_shared) {
        _shared = new nsEnv();
        return _shared->Init();
    }
    return true;
}

void nsEnv::Destroy() {
    mouseButtons.clear();
    mouseButtons.shrink_to_fit();
    delete _shared;
    _shared = nullptr;
}

void nsEnv::PrepareDesktopInput() {
    glfwSetScrollCallback(_wnd, [](GLFWwindow *window, double x, double y) -> void {
        auto ui = nsEngine::GetActiveInput();
        if (ui) ui->OnMouseWheel((float) y);
    });

    glfwSetCursorPosCallback(_wnd, [](GLFWwindow *window, double xpos, double ypos) -> void {
        nsEnv::Shared()->WindowToClientCoordinates(xpos, ypos);
        const nsVec2 pos{float(int(xpos)), float(int(ypos))};
        if (!mouseButtons.empty()) {
            for (auto button: mouseButtons) {
                nsEngine::OnPointerMove({POINTER_MOUSE, pos, button});
            }
        } else {
            nsEngine::OnPointerMove({POINTER_MOUSE, pos, -1});
        }
    });

    glfwSetMouseButtonCallback(_wnd, [](GLFWwindow *window, int button, int action, int mods) -> void {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        nsEnv::Shared()->WindowToClientCoordinates(x, y);
        const nsPointerData pointer{POINTER_MOUSE, {float(int(x)), float(int(y))}, button};
        if (action == GLFW_PRESS) {
            mouseButtons.push_back(button);
            nsEngine::OnPointerDown(pointer);
        } else if (action == GLFW_RELEASE) {
            auto it = std::remove(mouseButtons.begin(), mouseButtons.end(), button);
            if (it != mouseButtons.end()) {
                mouseButtons.erase(it);
            }
            nsEngine::OnPointerUp(pointer);
        }
    });
}

//---------------------------------------------------------
// nsEnv::OnWindowFocusChanged: cancel pointer captures before
// notifying the application so missed releases cannot survive focus loss.
//---------------------------------------------------------
void nsEnv::OnWindowFocusChanged( const bool focused ) {
    if (!focused && !mouseButtons.empty()) {
        double x = 0, y = 0;
        glfwGetCursorPos(_wnd, &x, &y);
        WindowToClientCoordinates(x, y);
        const nsVec2 pos{float(int(x)), float(int(y))};
        for ( const auto button : mouseButtons ) {
            nsEngine::OnPointerCancel({POINTER_MOUSE, pos, button});
        }
        mouseButtons.clear();
    }
    nsEngine::OnActivateApp( focused );
}
