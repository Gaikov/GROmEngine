//
// Created by Roman on 5/28/2024.
//

#ifndef _GROM_ANDROIDJAVAENV_H
#define _GROM_ANDROIDJAVAENV_H

#include "AndroidCommon.h"

class JavaObject {
public:
    typedef nsSmartPtr<JavaObject> sp_t;

    JavaObject(jobject object, JNIEnv *env, JavaVM *vm);
    jmethodID BeginMethod(const char *name, const char *sig);
    void EndMethod();

    JNIEnv* GetEnv() { return _env; }
    jobject GetObject() { return _object; }

private:
    jobject _object;
    JNIEnv *_env;
    JavaVM *_vm;
};


#endif //_GROM_ANDROIDJAVAENV_H
