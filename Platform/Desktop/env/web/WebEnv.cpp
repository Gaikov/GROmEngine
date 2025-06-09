//
// Created by Roman on 4/16/2025.
//

#include "env/Env.h"
#include "Engine/engine.h"
#include <emscripten/emscripten.h>
#include <emscripten/console.h>
#include <emscripten/html5.h>
#include "nsLib/log.h"

extern "C" {
	void nsEngine_OnActivateApp(bool active) {
		nsEngine::OnActivateApp(active);
	}
}

EM_JS(int, GetCanvasWidth, (), {
	return Module['canvas'].width;
});

EM_JS(int, GetCanvasHeight, (), {
	return Module['canvas'].height;
});

EM_JS(void, ShowCursorExternal, (bool show), {
	Module['canvas'].style.cursor = show ? 'default' : 'none';
});

EM_JS(bool, IsMobileExternal, (), {
  const isPhone = /Android|webOS|iPhone|iPad|iPod|BlackBerry|IEMobile|Opera Mini/ig.test(navigator.userAgent);
  const isTablet = /(ipad|tablet|playbook|silk)|(android(?!.*mobile))/ig.test(navigator.userAgent);
  const isTouchDevice = 'ontouchstart' in window || navigator.maxTouchPoints > 0;
  return isPhone || isTablet || isTouchDevice;
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
    Log::Info("http://grom-games.com");
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

EM_BOOL touch_callback(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData) {
	for (int i = 0; i < touchEvent->numTouches; i++) {
		const EmscriptenTouchPoint *touch = &touchEvent->touches[i];
		if (eventType == EMSCRIPTEN_EVENT_TOUCHSTART) {
			nsEngine::OnPointerDown(touch->identifier, touch->targetX, touch->targetY);
		} else if (eventType == EMSCRIPTEN_EVENT_TOUCHMOVE) {
			nsEngine::OnPointerMove(touch->identifier, touch->targetX, touch->targetY);
		} else {
			nsEngine::OnPointerUp(touch->identifier, touch->targetX, touch->targetY);
		}
	}

	return EM_TRUE; // Return EM_TRUE to prevent default event handling
}

void nsEnv::PrepareInput() {
  	if (IsMobile()) {
		emscripten_set_touchstart_callback("#canvas", NULL, 1, touch_callback);
		emscripten_set_touchmove_callback("#canvas", NULL, 1, touch_callback);
		emscripten_set_touchend_callback("#canvas", NULL, 1, touch_callback);
		emscripten_set_touchcancel_callback("#canvas", NULL, 1, touch_callback);
	} else {
        PrepareDesktopInput();
	}
}

void nsEnv::ShowCursor(bool show) const {
	ShowCursorExternal(show);
}
