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
    Log::Info("%s: %llu", _message, static_cast<unsigned long long>(delta));
}
