#ifndef _GROM_ANDROIDOUT_H
#define _GROM_ANDROIDOUT_H

#include <android/log.h>
#include <sstream>
#include "nsLib/log.h"

class AndroidLogPolicy : public ILogListener {
public:
    void LogPrint(LogLevel level, const char *str) override;
};

#endif //_GROM_ANDROIDOUT_H