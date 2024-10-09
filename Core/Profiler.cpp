//
// Created by Roman on 5/15/2024.
//

#include "Profiler.h"
#include "nsLib/log.h"
#include "engine/Platform.h"

nsProfilerSample::nsProfilerSample(const char *message) : _message(message) {
    _startTime = App_GetPlatform()->GetTime();
}

nsProfilerSample::~nsProfilerSample() {
    auto delta = App_GetPlatform()->GetTime() - _startTime;
    Log::Debug("%s: %i", _message, delta);
}
