#ifndef _LOG_H_
#define _LOG_H_

#include "headers.h"
#include "SubSystem.h"

#define MAX_MSG    (1024 * 50)

enum LogLevel
{
	PRN_DEV,        //debug
	PRN_ALL,        //Info
	PRN_WARNING,    //Warning
	PRN_ERROR,      //Error
};

struct ILogListener
{
	virtual void LogPrint(LogLevel level, const char *str) = 0; //TODO: add print level
};

void LogPrintf(LogLevel prnLevel, const char *fmt, ...);

class Log : public nsSubSystem<Log>
{
public:
	static void Info(const char *fmt, ...);
	static void Warning(const char *fmt, ...);
	static void Error(const char *fmt, ...);
	static void Debug(const char *fmt, ...);

public:
	void Print(LogLevel level, const char *msg);

public:
	void AddPolicy(ILogListener *policy);
	void RemovePolicy(ILogListener *policy);

private:
	std::list<ILogListener *>	_policies;

private:
	void PrintLine(LogLevel level, const char *title, const char *msg);
};


#endif