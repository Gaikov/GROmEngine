//
// Created by Roman on 4/16/2025.
//

#include "WebEnv.h"
#include "WebLog.h"
#include <emscripten/console.h>

bool nsWebEnv::Init() {
    Log::Init();
    Log::Info("Hello from GROm!");
    return true;
}

void nsWebEnv::MessagePopup(const char *title, const char *message) {
}

void nsWebEnv::OpenUrl(const char *url) {
}
