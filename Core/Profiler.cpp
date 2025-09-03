//
// Created by Roman on 5/15/2024.
//

#include "Profiler.h"
#include "nsLib/log.h"
#include "time/Time.h"

nsProfilerSample::nsProfilerSample(const char *message) : _message(message) {
    _startTime = nsTime::GetTimeMS();
}

nsProfilerSample::~nsProfilerSample() {
    const auto delta = nsTime::GetTimeMS() - _startTime;
    Log::Info("%s: %i", _message, delta);
}
