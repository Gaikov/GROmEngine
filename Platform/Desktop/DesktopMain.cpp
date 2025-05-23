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
#include "Core/Memory.h"
#include "env/Env.h"

static DesktopPlatform g_desktop;
static nsArgs g_args;

const nsArgs &DesktopPlatform::GetArgs() {
    return g_args;
}

Platform *App_GetPlatform() {
    return &g_desktop;
}

static bool success = false;

void onExit() {
    printf("======== AtExit =======\n");

    nsEngine::Release(!success);
    nsEnv::Destroy();

    nsVisualObject2d::LeaksCheck();
    mem_report();
}

int main(int argc, char *argv[]) {
    std::atexit(onExit);
    if (!nsEnv::Create()) {
        return -1;
    }

    g_args.FromArgs(argc, (const char **) argv);

    success = nsEngine::Init();
    if (success) {
        auto wnd = g_desktop.GetWindow();

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

        nsEnv::Shared()->PrepareInput();

        r_vsync->AddHandler(nsVar::NSVAR_CHANGED, [](const nsBaseEvent *e) {
            Log::Info("swap interval changed");
            glfwSwapInterval(r_vsync->Bool() ? 1 : 0);
        });
        glfwSwapInterval(r_vsync->Bool() ? 1 : 0);

        nsEnv::Shared()->MainLoop();
    }

    return 0;
}
