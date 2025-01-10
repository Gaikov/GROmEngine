//
// Created by Roman on 5/3/2024.
//
#include "DesktopPlatform.h"
#include "nsLib/log.h"
#include "Engine/engine.h"
#include "Core/sys.h"
#include "Engine/RenDevice.h"
#include "Core/Var.h"
#include "Engine/display/VisualObject2d.h"

static DesktopPlatform g_desktop;
static nsArgs g_args;
static std::vector<int> mouseButtons;

const nsArgs &DesktopPlatform::GetArgs() {
    return g_args;
}

Platform *App_GetPlatform() {
    return &g_desktop;
}

void onExit() {
    printf("======== AtExit =======\n");
    nsVisualObject2d::LeaksCheck();
}

int main(int argc, char *argv[]) {
    std::atexit(onExit);

    g_args.FromArgs(argc, (const char **) argv);

    bool success = nsEngine::Init();
    if (success) {
        auto wnd = g_desktop.GetWindow();

        glfwSetWindowSizeCallback(wnd, [](GLFWwindow *window, int width, int height) {
            glViewport(0, 0, width, height);
        });

        glfwSetJoystickCallback([](int id, int status) {
            if (status == GLFW_CONNECTED) {
                Log::Info("gamepad connected: %s", glfwGetJoystickName(id));
            } else if (status == GLFW_DISCONNECTED) {
                Log::Info("gamepad disconnected: %s", glfwGetJoystickName(id));
            }
        });

        glfwSetWindowFocusCallback(wnd, [](GLFWwindow *window, int focused) {
            nsEngine::OnActivateApp(focused);
        });

        glfwSetKeyCallback(wnd,
                           [](GLFWwindow *window, int key, int scancode, int action, int mods) -> void {
                               if (action == GLFW_PRESS) {
                                   //Log::Info("key: %i, scan: %i", key, scancode);
                                   nsEngine::OnKeyDown(key, false);
                               }
                               if (action == GLFW_REPEAT) {
                                   nsEngine::OnKeyDown(key, true);
                               } else if (action == GLFW_RELEASE) {
                                   nsEngine::OnKeyUp(key);
                               }
                           });

        glfwSetCharCallback(wnd, [](GLFWwindow *window, unsigned int codepoint) -> void {
            nsEngine::OnCharDown((char) codepoint);
        });

        glfwSetScrollCallback(wnd, [](GLFWwindow *window, double x, double y) -> void {
            auto ui = nsEngine::GetActiveInput();
            if (ui) ui->OnMouseWheel((float) y);
        });

        glfwSetCursorPosCallback(wnd, [](GLFWwindow *window, double xpos, double ypos) -> void {
            if (!mouseButtons.empty()) {
                for (auto button: mouseButtons) {
                    nsEngine::OnPointerMove(button, (int) xpos, (int) ypos);
                }
            } else {
                nsEngine::OnPointerMove(-1, (int) xpos, (int) ypos);
            }
        });

        glfwSetMouseButtonCallback(wnd, [](GLFWwindow *window, int button, int action, int mods) -> void {
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            if (action == GLFW_PRESS) {
                mouseButtons.push_back(button);
                nsEngine::OnPointerDown(button, (int) x, (int) y);
            } else if (action == GLFW_RELEASE) {
                auto it = std::remove(mouseButtons.begin(), mouseButtons.end(),button);
                if (it != mouseButtons.end()) {
                    mouseButtons.erase(it);
                }
                nsEngine::OnPointerUp(button, (int) x, (int) y);
            }
        });

        r_vsync->AddHandler(nsVar::NSVAR_CHANGED, [](const nsBaseEvent *e) {
            Log::Info("swap interval changed");
            glfwSwapInterval(r_vsync->Bool() ? 1 : 0);
        });
        glfwSwapInterval(r_vsync->Bool() ? 1 : 0);

        while (!glfwWindowShouldClose(wnd) && !Sys_IsExit()) {
            nsEngine::MainLoop();
            glfwPollEvents();
        }
    }

    nsEngine::Release(!success);
    glfwTerminate();
    return 0;
}
