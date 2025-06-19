//
// Created by Roman on 5/15/2024.
//

#include "Profiler.h"
#include "nsLib/log.h"
#include "Engine/Platform.h"

nsProfilerSample::nsProfilerSample(const char *message) : _message(message) {
    _startTime = GetTime();
}

nsProfilerSample::~nsProfilerSample() {
    const auto delta = GetTime() - _startTime;
    Log::Info("%s: %i", _message, delta);
}

long long nsProfilerSample::GetTime() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}
