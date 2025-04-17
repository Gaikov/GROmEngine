//
// Created by Roman on 4/16/2025.
//

#include "WebEnv.h"
#include "WebLog.h"
#include "Engine/engine.h"
#include <emscripten/emscripten.h>
#include <emscripten/console.h>

static void Loop() {
	nsEngine::MainLoop();
	glfwPollEvents();
}

bool nsWebEnv::Init() {
    Log::Init();
    Log::Info("Hello from GROm!");
    return true;
}

void nsWebEnv::MessagePopup(const char *title, const char *message) {
}

void nsWebEnv::OpenUrl(const char *url) {
}

void nsWebEnv::MainLoop(GLFWwindow *wnd) {
	emscripten_set_main_loop(Loop, 0, true);
}
