//
// Created by Roman on 4/16/2025.
//

#include "WebLog.h"
#include <emscripten/console.h>

nsWebLog::nsWebLog() {
    emscripten_console_log("...web log created");
}

void nsWebLog::LogPrint(LogLevel level, const char *str) {
    emscripten_console_log(str);
}
