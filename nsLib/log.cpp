#include "headers.h"
#include "log.h"

static const char *FormatLine(const char *prefix, const char *fmt, va_list list) {
    thread_local char buf[MAX_MSG];
    int offset = snprintf(buf, MAX_MSG, "%s", prefix);
    int n = vsnprintf(buf + offset, MAX_MSG - offset - 1, fmt, list);
    int total = offset + (n > 0 ? n : 0);
    if (total < MAX_MSG - 1) {
        buf[total] = '\n';
        buf[total + 1] = '\0';
    }
    return buf;
}

void Log::AddPolicy(ILogListener *policy) {
    _policies.push_back(policy);
}

void Log::RemovePolicy(ILogListener *policy) {
    auto it = std::find(_policies.begin(), _policies.end(), policy);
    if (it != _policies.end()) {
        _policies.erase(it);
    }
}

void Log::OnRelease() {
    for (const auto policy: _policies) {
        delete policy;
    }
    _policies.clear();

    nsSubSystem::OnRelease();
}

void Log::Print(LogLevel level, const char *msg) {
#ifdef NDEBUG
    if (level == PRN_DEV) {
        return;
    }
#endif
    for (auto policy: _policies) {
        policy->LogPrint(level, msg);
    }
}

void Log::Info(const char *fmt, ...) {
    va_list list;
    va_start(list, fmt);
    Shared()->Print(PRN_ALL, FormatLine("", fmt, list));
    va_end(list);
}

void Log::Warning(const char *fmt, ...) {
    va_list list;
    va_start(list, fmt);
    Shared()->Print(PRN_WARNING, FormatLine("WARNING: ", fmt, list));
    va_end(list);
}

void Log::Error(const char *fmt, ...) {
    va_list list;
    va_start(list, fmt);
    Shared()->Print(PRN_ERROR, FormatLine("ERROR: ", fmt, list));
    va_end(list);
}

void Log::Debug(const char *fmt, ...) {
#ifdef DEBUG
    va_list list;
    va_start(list, fmt);
    Shared()->Print(PRN_DEV, FormatLine("", fmt, list));
    va_end(list);
#endif
}

void LogPrintf(LogLevel prnLevel, const char *fmt, ...) {
    va_list list;
    va_start(list, fmt);
    Log::Shared()->Print(prnLevel, FormatLine("", fmt, list));
    va_end(list);
}
