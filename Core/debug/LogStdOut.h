//
// Created by Roman Gaikov on 2020-05-12.
//

#ifndef __LOGSTDOUT_H__
#define __LOGSTDOUT_H__

#include "nsLib/log.h"

class LogStdOut : public ILogListener
{
public:
	void LogPrint(LogLevel level, const char *str) override;

private:
	std::mutex _mutex;
};


#endif //__LOGSTDOUT_H__
