#include "headers.h"
#include "log.h"

#define va_print(format, msg) \
    va_list list; \
	va_start(list, format); \
	vsprintf(msg, format, list); \
	va_end(list)

void Log::AddPolicy(ILogListener *policy)
{
	_policies.push_back(policy);
}

void Log::RemovePolicy(ILogListener *policy)
{
	auto it = std::find(_policies.begin(), _policies.end(), policy);
	if (it != _policies.end())
	{
		_policies.erase(it);
	}
}

void Log::Print(LogLevel level, const char *msg)
{
#ifdef NDEBUG
	if (level == PRN_DEV)
	{
		return;
	}
#endif
	for (auto policy : _policies)
	{
		policy->LogPrint(level, msg);
	}
}

void Log::PrintLine(LogLevel level, const char *title, const char *msg)
{
	char line[MAX_MSG];
	sprintf(line, "%s%s\n", title, msg);
	Print(level, line);
}

void Log::Info(const char *fmt, ...)
{
	char message[MAX_MSG];
	va_print(fmt, message);
	Shared()->PrintLine(PRN_ALL, "", message);
}

void Log::Warning(const char *fmt, ...)
{
	char message[MAX_MSG];
	va_print(fmt, message);
	Shared()->PrintLine(PRN_WARNING, "WARNING: ", message);
}

void Log::Error(const char *fmt, ...)
{
	char message[MAX_MSG];
	va_print(fmt, message);
	Shared()->PrintLine(PRN_ERROR, "ERROR: ", message);
}

void Log::Debug(const char *fmt, ...) {
#ifdef DEBUG
    char message[MAX_MSG];
    va_print(fmt, message);
    Shared()->PrintLine(PRN_DEV, "", message);
#endif
}

//---------------------------------------------------------
// log_printf: 
//---------------------------------------------------------
void LogPrintf(LogLevel prnLevel, const char *fmt, ...)
{
	if (!strlen(fmt)) return;

	char    msg[MAX_MSG];
	va_print(fmt, msg);
	Log::Shared()->Print(prnLevel, msg);
}
