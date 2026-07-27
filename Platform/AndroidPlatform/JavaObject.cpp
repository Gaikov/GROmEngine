//
// Created by Roman on 5/28/2024.
//

#include "JavaObject.h"
#include "nsLib/log.h"

namespace {
class ScopedJniEnv {
public:
    explicit ScopedJniEnv(JavaVM *vm) : _vm(vm) {
        const jint status = _vm->GetEnv(reinterpret_cast<void **>(&_env), JNI_VERSION_1_6);
        if (status == JNI_EDETACHED) {
            if (_vm->AttachCurrentThread(&_env, nullptr) == JNI_OK) {
                _attached = true;
            } else {
                _env = nullptr;
            }
        } else if (status != JNI_OK) {
            _env = nullptr;
        }
    }

    ~ScopedJniEnv() {
        if (_attached) {
            _vm->DetachCurrentThread();
        }
    }

    JNIEnv *Get() const { return _env; }

private:
    JavaVM *_vm;
    JNIEnv *_env = nullptr;
    bool _attached = false;
};

bool ClearJavaException(JNIEnv *env, const char *methodName) {
    if (!env->ExceptionCheck()) {
        return false;
    }
    env->ExceptionDescribe();
    env->ExceptionClear();
    Log::Warning("Java exception while calling '%s'", methodName);
    return true;
}

class LocalClass {
public:
    LocalClass(JNIEnv *env, jobject object) : _env(env), _class(env->GetObjectClass(object)) {}
    ~LocalClass() {
        if (_class) {
            _env->DeleteLocalRef(_class);
        }
    }
    jclass Get() const { return _class; }

private:
    JNIEnv *_env;
    jclass _class;
};

jmethodID FindMethod(
        JNIEnv *env, jobject object, const char *name, const char *signature) {
    LocalClass cls(env, object);
    if (!cls.Get()) {
        ClearJavaException(env, name);
        Log::Warning("Can't get the Java object class for method '%s'", name);
        return nullptr;
    }

    const jmethodID method = env->GetMethodID(cls.Get(), name, signature);
    if (ClearJavaException(env, name) || !method) {
        Log::Warning("Can't find Java method '%s'", name);
        return nullptr;
    }
    return method;
}
}

JavaObject::JavaObject(jobject object, JavaVM *vm) : _object(nullptr), _vm(vm) {
    ScopedJniEnv scopedEnv(_vm);
    if (JNIEnv *env = scopedEnv.Get()) {
        _object = env->NewGlobalRef(object);
    }
    if (!_object) {
        Log::Error("Can't create a global reference to the Android activity");
    }
}

JavaObject::~JavaObject() {
    if (!_object) {
        return;
    }
    ScopedJniEnv scopedEnv(_vm);
    if (JNIEnv *env = scopedEnv.Get()) {
        env->DeleteGlobalRef(_object);
    }
}

bool JavaObject::CallBoolean(const char *name, bool &result) const {
    ScopedJniEnv scopedEnv(_vm);
    JNIEnv *env = scopedEnv.Get();
    if (!env || !_object) {
        return false;
    }

    const jmethodID method = FindMethod(env, _object, name, "()Z");
    if (!method) {
        return false;
    }

    result = env->CallBooleanMethod(_object, method) == JNI_TRUE;
    return !ClearJavaException(env, name);
}

bool JavaObject::CallVoidString(const char *name, const char *value) const {
    ScopedJniEnv scopedEnv(_vm);
    JNIEnv *env = scopedEnv.Get();
    if (!env || !_object) {
        return false;
    }

    const jmethodID method =
            FindMethod(env, _object, name, "(Ljava/lang/String;)V");
    if (!method) {
        return false;
    }

    jstring javaValue = env->NewStringUTF(value ? value : "");
    if (!javaValue || ClearJavaException(env, name)) {
        return false;
    }
    env->CallVoidMethod(_object, method, javaValue);
    env->DeleteLocalRef(javaValue);
    return !ClearJavaException(env, name);
}

bool JavaObject::CallVoidStrings(
        const char *name, const char *first, const char *second) const {
    ScopedJniEnv scopedEnv(_vm);
    JNIEnv *env = scopedEnv.Get();
    if (!env || !_object) {
        return false;
    }

    const jmethodID method = FindMethod(
            env, _object, name, "(Ljava/lang/String;Ljava/lang/String;)V");
    if (!method) {
        return false;
    }

    jstring javaFirst = env->NewStringUTF(first ? first : "");
    if (!javaFirst || ClearJavaException(env, name)) {
        return false;
    }
    jstring javaSecond = env->NewStringUTF(second ? second : "");
    if (!javaSecond || ClearJavaException(env, name)) {
        env->DeleteLocalRef(javaFirst);
        return false;
    }
    env->CallVoidMethod(_object, method, javaFirst, javaSecond);
    env->DeleteLocalRef(javaFirst);
    env->DeleteLocalRef(javaSecond);
    return !ClearJavaException(env, name);
}
