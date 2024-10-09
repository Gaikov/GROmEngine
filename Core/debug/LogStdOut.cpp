//
// Created by Roman Gaikov on 2020-05-12.
//

#include "LogStdOut.h"

void LogStdOut::LogPrint(LogLevel level, const char *str)
{
	printf("%s", str);
    fflush(stdout);
}
