#include <cstdlib>
#include <string>
#include <vector>

#include <jni.h>

#include "Engine/Platform.h"
#include "Platform/AndroidPlatform/AndroidPlatform.h"
#include "Share/ShareBackend.h"

namespace {
class AndroidShareBackend final : public nsShareService::IBackend {
public:
    using RequestId = nsShareService::RequestId;

    bool Share(RequestId requestId, const nsShareRequest &request) override {
        auto *platform = dynamic_cast<AndroidPlatform *>(App_GetPlatform());
        if (!platform) return false;

        return platform->HandlePlatformCommand("share.open", {
                std::to_string(requestId),
                request.title,
                request.text,
                request.url,
                request.imageAssetPath
        });
    }

    void CancelAll() override {
    }
};

std::string ReadString(JNIEnv *env, jstring value) {
    if (!value) return {};
    const auto *chars = env->GetStringUTFChars(value, nullptr);
    if (!chars) return {};
    std::string result(chars);
    env->ReleaseStringUTFChars(value, chars);
    return result;
}
}

extern "C" JNIEXPORT void JNICALL
Java_com_grom_engine_ShareBridge_nativeComplete(
        JNIEnv *env, jclass, jlong requestId, jint status, jstring diagnostic) {
    auto result = nsShareResult {
            static_cast<nsShareStatus>(status),
            ReadString(env, diagnostic)
    };
    nsShareService::Complete(
            static_cast<nsShareService::RequestId>(requestId), std::move(result));
}

nsShareService::IBackend *CreateShareBackend() {
    return new AndroidShareBackend();
}
