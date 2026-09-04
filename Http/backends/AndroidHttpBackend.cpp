#include <jni.h>

#include "Http/HttpBackend.h"
#include "nsLib/log.h"

namespace {
JavaVM *g_vm = nullptr;
jclass g_bridgeClass = nullptr;
jmethodID g_sendMethod = nullptr;

class ScopedEnv {
public:
    ScopedEnv() {
        if (!g_vm) return;
        const auto result = g_vm->GetEnv(reinterpret_cast<void **>(&_env), JNI_VERSION_1_6);
        if (result == JNI_EDETACHED && g_vm->AttachCurrentThread(&_env, nullptr) == JNI_OK) {
            _attached = true;
        } else if (result != JNI_OK) {
            _env = nullptr;
        }
    }

    ~ScopedEnv() {
        if (_attached) g_vm->DetachCurrentThread();
    }

    JNIEnv *Get() const { return _env; }

private:
    JNIEnv *_env = nullptr;
    bool _attached = false;
};

class AndroidHttpBackend final : public nsHttpClient::IBackend {
public:
    using RequestId = nsHttpClient::RequestId;

    bool Send(RequestId requestId, const nsHttpRequest &request) override {
        ScopedEnv scoped;
        auto *env = scoped.Get();
        if (!env || !g_bridgeClass || !g_sendMethod) return false;

        auto method = env->NewStringUTF(request.method.c_str());
        auto url = env->NewStringUTF(request.url.c_str());
        auto body = env->NewStringUTF(request.body.c_str());
        auto stringClass = env->FindClass("java/lang/String");
        auto headers = env->NewObjectArray(
                static_cast<jsize>(request.headers.size() * 2), stringClass, nullptr);
        for (jsize i = 0; i < static_cast<jsize>(request.headers.size()); ++i) {
            auto name = env->NewStringUTF(request.headers[i].first.c_str());
            auto value = env->NewStringUTF(request.headers[i].second.c_str());
            env->SetObjectArrayElement(headers, i * 2, name);
            env->SetObjectArrayElement(headers, i * 2 + 1, value);
            env->DeleteLocalRef(name);
            env->DeleteLocalRef(value);
        }

        env->CallStaticVoidMethod(g_bridgeClass, g_sendMethod,
                static_cast<jlong>(requestId), method, url, headers, body,
                static_cast<jint>(request.timeoutMs));
        const bool success = !env->ExceptionCheck();
        if (!success) {
            env->ExceptionDescribe();
            env->ExceptionClear();
            Log::Warning("Java exception while starting HTTP request");
        }
        env->DeleteLocalRef(headers);
        env->DeleteLocalRef(stringClass);
        env->DeleteLocalRef(body);
        env->DeleteLocalRef(url);
        env->DeleteLocalRef(method);
        return success;
    }

    void CancelAll() override {
    }
};
}

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *) {
    g_vm = vm;
    JNIEnv *env = nullptr;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    auto localClass = env->FindClass("com/grom/engine/HttpBridge");
    if (!localClass) {
        env->ExceptionClear();
        return JNI_ERR;
    }
    g_bridgeClass = static_cast<jclass>(env->NewGlobalRef(localClass));
    env->DeleteLocalRef(localClass);
    if (!g_bridgeClass) return JNI_ERR;
    g_sendMethod = env->GetStaticMethodID(g_bridgeClass, "send",
            "(JLjava/lang/String;Ljava/lang/String;[Ljava/lang/String;Ljava/lang/String;I)V");
    if (!g_sendMethod) {
        env->ExceptionClear();
        return JNI_ERR;
    }
    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNICALL
Java_com_grom_engine_HttpBridge_nativeComplete(
        JNIEnv *env, jclass, jlong requestId, jint statusCode, jint errorCode,
        jstring body, jstring diagnostic) {
    auto read = [env](jstring value) -> std::string {
        if (!value) return {};
        const auto *chars = env->GetStringUTFChars(value, nullptr);
        if (!chars) return {};
        std::string result(chars);
        env->ReleaseStringUTFChars(value, chars);
        return result;
    };

    nsHttpResponse response;
    response.statusCode = statusCode;
    response.error = errorCode == 0 ? nsHttpError::None
            : (errorCode == 2 ? nsHttpError::Timeout : nsHttpError::Network);
    response.body = read(body);
    response.diagnostic = read(diagnostic);
    nsHttpClient::Complete(static_cast<nsHttpClient::RequestId>(requestId), std::move(response));
}

nsHttpClient::IBackend *CreateHttpBackend() {
    return new AndroidHttpBackend();
}
