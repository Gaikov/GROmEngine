//
// Created by Roman on 5/28/2024.
//

#include "JavaObject.h"
#include "nsLib/log.h"

JavaObject::JavaObject(jobject object, JNIEnv *env, JavaVM *vm) :
        _object(object),
        _env(env), 
        _vm(vm) {
}

jmethodID JavaObject::BeginMethod(const char *name, const char *sig) {
    if (_vm->AttachCurrentThread(&_env, nullptr) != JNI_OK) {
        Log::Error("Can't java to attach current thread!");
        return nullptr;
    }

    auto cls = _env->GetObjectClass(_object);
    if (!cls) {
        Log::Warning("Can't get jobject class for method '%s'!", name);
        _vm->DetachCurrentThread();
        return nullptr;
    }

    auto method = _env->GetMethodID(cls, name, sig);
    if (!method) {
        Log::Warning("Can't get jobject method '%s'!", name);
        _vm->DetachCurrentThread();
    }

    return method;
}

void JavaObject::EndMethod() {
    _vm->DetachCurrentThread();
}
