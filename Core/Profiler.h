//
// Created by Roman on 5/15/2024.
//

#ifndef _GROM_PROFILER_H
#define _GROM_PROFILER_H

class nsProfilerSample {
public:
    explicit nsProfilerSample(const char *message);
    virtual ~nsProfilerSample();
private:
    const char *_message;
    unsigned int _startTime;
};

#endif //_GROM_PROFILER_H
