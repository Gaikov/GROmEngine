#include "AndroidLogPolicy.h"


void AndroidLogPolicy::LogPrint(LogLevel level, const char *str) {
    std::map<LogLevel, int> map;
    map[PRN_ALL] = ANDROID_LOG_INFO;
    map[PRN_DEV] = ANDROID_LOG_DEBUG;
    map[PRN_ERROR] = ANDROID_LOG_ERROR;
    map[PRN_WARNING] = ANDROID_LOG_WARN;

    __android_log_print(map[level], "GROM", "%s", str);
}
