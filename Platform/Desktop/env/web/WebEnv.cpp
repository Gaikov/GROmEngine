//
// Created by Roman on 4/16/2025.
//

#include "env/Env.h"
#include "Engine/engine.h"
#include <emscripten/emscripten.h>
#include <emscripten/console.h>
#include "nsLib/log.h"

EM_JS(int, GetCanvasWidth, (), {
	return Module['canvas'].width;
});

EM_JS(int, GetCanvasHeight, (), {
	return Module['canvas'].height;
});

static void Loop() {
	nsEngine::MainLoop();
	glfwPollEvents();
}

bool nsEnv::Init() {
    Log::Init();
    Log::Info("Hello from GROm!");
    return true;
}

void nsEnv::MessagePopup(const char *title, const char *message) {
}

void nsEnv::OpenUrl(const char *url) {
}

void nsEnv::MainLoop() {
	glfwSetWindowSize(_wnd, GetCanvasWidth(), GetCanvasHeight());
	emscripten_set_main_loop(Loop, 0, true);
}

void nsEnv::GetClientSize(int &width, int &height) {
	width = GetCanvasWidth();
	height = GetCanvasHeight();
}

GLFWwindow* nsEnv::CreateGameWindow() {
	_wnd = glfwCreateWindow(GetCanvasWidth(), GetCanvasHeight(), "GROm Engine", nullptr, nullptr);
	return _wnd;
}