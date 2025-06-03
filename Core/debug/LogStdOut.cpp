//
// Created by Roman Gaikov on 2020-05-12.
//

#include "LogStdOut.h"

void LogStdOut::LogPrint(LogLevel level, const char *str)
{
	std::lock_guard lock(_mutex);
	printf("%s", str);
    fflush(stdout);
}
