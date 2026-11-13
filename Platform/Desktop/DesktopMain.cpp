//
// Created by Roman on 5/3/2024.
//
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#endif

#include "DesktopPlatform.h"
#include "nsLib/log.h"
#include "Engine/engine.h"
#include "Core/sys.h"
#include "Engine/RenDevice.h"
#include "Core/Var.h"
#include "Engine/display/VisualObject2d.h"
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

static void Shutdown() {
    static bool released = false;
    if (released) return;
    released = true;

    printf("======== Shutdown =======\n");

    nsEngine::Release(!success);
    nsEnv::Destroy();

    nsVisualObject2d::LeaksCheck();
}

static int RunDesktopApp(int argc, char *argv[]) {
    std::atexit(Shutdown);
    if (!nsEnv::Create()) {
        Shutdown();
        return -1;
    }

    g_args.FromArgs(argc, (const char **) argv);

    success = nsEngine::Init();
    if (success) {
        auto wnd = g_desktop.GetWindow();

        glfwPollEvents();

#ifndef WEB_ASM
        glfwSetJoystickCallback([](int id, int status) {
            if (status == GLFW_DISCONNECTED) g_desktop.OnGamepadDisconnected(id);
            else if (status == GLFW_CONNECTED) {
                int axes = 0, buttons = 0;
                glfwGetJoystickAxes(id, &axes);
                glfwGetJoystickButtons(id, &buttons);
                g_desktop.OnGamepadConnected(id, glfwGetJoystickName(id), axes, buttons);
            }
        });
#endif

        glfwSetWindowFocusCallback(wnd, [](GLFWwindow *window, int focused) {
            nsEnv::Shared()->OnWindowFocusChanged( focused != 0 );
        });

#ifndef WEB_ASM
        glfwSetKeyCallback(wnd,
                           [](GLFWwindow *window, const int key, int scancode, const int action, const int mods) -> void {
                               if (action == GLFW_PRESS) {
                                   //Log::Info("key: %i, scan: %i", key, scancode);
                                   nsEngine::OnKeyDown(key, false, mods);
                               }
                               if (action == GLFW_REPEAT) {
                                   nsEngine::OnKeyDown(key, true, mods);
                               } else if (action == GLFW_RELEASE) {
                                   nsEngine::OnKeyUp(key, mods);
                               }
                           });

        glfwSetCharCallback(wnd, [](GLFWwindow *window, unsigned int codepoint) -> void {
            nsEngine::OnCharDown((char) codepoint);
        });
#endif

        nsEnv::Shared()->PrepareInput();

        r_vsync->AddHandler(nsVar::NSVAR_CHANGED, [](const nsBaseEvent *e) {
            Log::Info("swap interval changed");
            glfwSwapInterval(r_vsync->Bool() ? 1 : 0);
        });
        glfwSwapInterval(r_vsync->Bool() ? 1 : 0);

        nsEnv::Shared()->MainLoop();
    }

    g_args.Clear();
    // Release application integrations before process finalizers run.
    Shutdown();
    return 0;
}

int main(int argc, char *argv[]) {
    return RunDesktopApp(argc, argv);
}

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    return RunDesktopApp(__argc, __argv);
}
#endif
