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
	virtual ~ILogListener() = default;
	virtual void LogPrint(LogLevel level, const char *str) = 0;
};

void LogPrintf(LogLevel prnLevel, const char *fmt, ...) NS_PRINTF_FORMAT(2, 3);

class Log : public nsSubSystem<Log>
{
public:
	static void Info(const char *fmt, ...) NS_PRINTF_FORMAT(1, 2);
	static void Warning(const char *fmt, ...) NS_PRINTF_FORMAT(1, 2);
	static void Error(const char *fmt, ...) NS_PRINTF_FORMAT(1, 2);
	static void Debug(const char *fmt, ...) NS_PRINTF_FORMAT(1, 2);

public:
	void Print(LogLevel level, const char *msg);

public:
	void AddPolicy(ILogListener *policy);
	void RemovePolicy(ILogListener *policy);

protected:
	void OnRelease() override;

private:
	std::list<ILogListener *>	_policies;

};


#endif
