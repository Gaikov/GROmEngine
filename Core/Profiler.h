//
// Created by Roman on 5/15/2024.
//

#ifndef _GROM_PROFILER_H
#define _GROM_PROFILER_H

class nsProfilerSample {
public:
    explicit nsProfilerSample(const char *message);
    virtual ~nsProfilerSample();
    static long long GetTime();

private:
    const char *_message;
    long long _startTime;
};

#endif //_GROM_PROFILER_H
