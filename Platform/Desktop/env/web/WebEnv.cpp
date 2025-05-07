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

EM_JS(bool, IsMobileExternal, (), {
  return /Android|webOS|iPhone|iPad|iPod|BlackBerry|IEMobile|Opera Mini/i.test(navigator.userAgent);
});

EM_JS(void, OpenUrlExternal, (const char* str), {
  const url = UTF8ToString(str);
  window.open(url, '_blank');
});

EM_JS(const char*, GetDomainNameExternal, (), {
  var str = document.location.host || document.location.hostname || "";
  var lengthBytes = lengthBytesUTF8(str) + 1;
  var ptr = _malloc(lengthBytes);
  stringToUTF8(str, ptr, lengthBytes);
  return ptr;
});

static void Loop() {
	nsEngine::MainLoop();
	glfwPollEvents();
}

bool nsEnv::Init() {
    Log::Init();
    Log::Info("Hello from GROm!");
	_domainName = emscripten_run_script_string("window.location.hostname");
    return true;
}

void nsEnv::MessagePopup(const char *title, const char *message) {
}

void nsEnv::OpenUrl(const char *url) {
	OpenUrlExternal(url);
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

bool nsEnv::IsMobile() {
	return IsMobileExternal();
}