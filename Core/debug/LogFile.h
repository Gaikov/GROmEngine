//
// Created by Roman Gaikov on 2019-10-11.
//

#ifndef __LOGFILE_H__
#define __LOGFILE_H__

//
// Created by Roman Gaikov on 2019-10-11.
//
#include "Config.h"
#include "AppInfo.h"
#include "nsLib/log.h"

//---------------------------------------------------------
// nsAutoLog:
//---------------------------------------------------------
class nsLogFile : public ILogListener
{
public:
	bool Open(const char *fileName);
	void Close();

private:
	FILE *m_pLog = nullptr;
	std::mutex _mutex;

private:
	void LogPrint(LogLevel level, const char *str) override;
};

extern nsLogFile g_log;


#endif //__LOGFILE_H__
