// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file WebLog.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "nsLib/log.h"

class nsWebLog : public ILogListener {
public:
    nsWebLog();
    void LogPrint(LogLevel level, const char *str) override;
};
