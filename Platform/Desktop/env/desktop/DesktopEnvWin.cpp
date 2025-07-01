//
// Created by roman on 7/1/25.
//
#include <windows.h>
#include "env/Env.h"

void nsEnv::MessagePopup(const char *title, const char *message) {
    ::MessageBox(nullptr, message, title, 0);
}

void nsEnv::OpenUrl(const char *url) {
    ShellExecute(nullptr, "open", url, nullptr, nullptr, SW_SHOWNORMAL);
}
