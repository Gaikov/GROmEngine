//
// Created by Roman Gaikov on 2019-10-11.
//
#include "LogFile.h"
#include <time.h>

nsLogFile g_log;

bool nsLogFile::Open(const char *fileName)
{
	std::lock_guard lock(_mutex);
	if (!(m_pLog = fopen(fileName, "w")))
		return false;

	Log::Shared()->AddPolicy(this);
	time_t t     = time(nullptr);
	fprintf(m_pLog, "open log: %s\n", ctime(&t));
	return true;
}

void nsLogFile::Close()
{
	if (m_pLog)
	{
		std::lock_guard lock(_mutex);
		time_t t = time(nullptr);
		fprintf(m_pLog, "close log: %s", ctime(&t));
		fclose(m_pLog);
		m_pLog = nullptr;
		Log::Shared()->RemovePolicy(this);
	}
}

void nsLogFile::LogPrint(LogLevel level, const char *str)
{
	if (m_pLog)
	{
		std::lock_guard lock(_mutex);
		fprintf(m_pLog, "%s", str);
		fflush(m_pLog);
	}
}
