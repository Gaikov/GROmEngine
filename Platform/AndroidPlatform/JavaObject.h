//
// Created by Roman on 5/28/2024.
//

#ifndef _GROM_ANDROIDJAVAENV_H
#define _GROM_ANDROIDJAVAENV_H

#include "AndroidCommon.h"

class JavaObject {
public:
    typedef nsSmartPtr<JavaObject> sp_t;

    JavaObject(jobject object, JavaVM *vm);
    ~JavaObject();

    bool CallBoolean(const char *name, bool &result) const;
    bool CallVoidString(const char *name, const char *value) const;
    bool CallVoidStrings(const char *name, const char *first, const char *second) const;

private:
    jobject _object;
    JavaVM *_vm;
};


#endif //_GROM_ANDROIDJAVAENV_H
