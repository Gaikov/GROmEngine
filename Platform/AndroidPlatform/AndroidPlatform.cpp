//
// Created by Roman on 4/2/2024.
//

#include "AndroidPlatform.h"
#include "AndroidAssetReader.h"
#include "nsLib/log.h"
#include "AndroidImagesDecoder.h"
#include "Core/Package.h"
#include "Core/FileReader.h"
#include "Core/FileWriter.h"
#include "Engine/input/SoftInputEmpty.h"
#include "Engine/input/soft/SoftInputKeyboard.h"

static nsArgs g_args;
static AndroidPlatform *g_platform = nullptr;

Platform *App_GetPlatform() {
    return g_platform;
}

bool AndroidPlatform::Create(JNIEnv *jenv, JavaVM *vm, AAssetManager *assetManager,
                             const char *internalDataPath) {
    assert(g_platform == nullptr);
    Log::Info("...Creating Android Platform");
    g_platform = new AndroidPlatform();
    g_platform->_assetManager = assetManager;
    g_platform->_internalDataPath = internalDataPath;
    return true;
}

void AndroidPlatform::Destroy() {
    delete g_platform;
    g_platform = nullptr;
}

void AndroidPlatform::SwapBuffers() {
    if (_glContext) {
        _glContext->SwapBuffers();
    }
}

AndroidPlatform::AndroidPlatform() :
        _assetManager(nullptr),
        _screenWidth(0),
        _screenHeight(0),
        _glContext(nullptr) {
    _softInput = new nsSoftInputKeyboard();
}

bool AndroidPlatform::Init() {
    return true;
}

void AndroidPlatform::Release() {
    delete g_platform;
    g_platform = nullptr;
}

const nsArgs &AndroidPlatform::GetArgs() {
    return g_args;
}

void AndroidPlatform::GetCursorPos(int &x, int &y) {

}

void AndroidPlatform::SetCursorPos(int x, int y) {

}

void AndroidPlatform::ShowCursor(bool show) {

}

bool AndroidPlatform::IsKeyPressed(int key) {
    return _keyboard.IsKeyPressed(key);
}

const char *AndroidPlatform::GetKeyName(int key) {
    return _keyboard.GetName(key);
}

unsigned int AndroidPlatform::GetTime() {
    return std::chrono::system_clock::now().time_since_epoch().count() / 1000;
}

void AndroidPlatform::Minimize() {

}

void AndroidPlatform::SetAppTitle(const char *text) {

}

void AndroidPlatform::GetClientSize(int &width, int &height) {
    width = _screenWidth;
    height = _screenHeight;
}

void *AndroidPlatform::GetWindowHandler() {
    return nullptr;
}

void AndroidPlatform::GetDisplayModes(Platform::tDisplayModesList &modes) {
    DisplayMode m = {
            _screenWidth,
            _screenHeight,
            0,
            0
    };

    modes.push_back(m);
}

bool AndroidPlatform::ApplyDisplayMode(
        int width, int height, bool fullScreen, int bits, int frequency) {

    return true;
}

IDataReader *AndroidPlatform::FileRead(const char *fileName) {
    return new AndroidAssetReader(_assetManager, fileName);
}

bool AndroidPlatform::FolderListing(const nsFilePath &folder, nsFilePath::tList &result) {
    const char *path = folder.GetPath() == "./" ? "" : folder.GetPath();
    auto dir = AAssetManager_openDir(_assetManager, path);
    if (!dir) {
        Log::Warning("can't open dir %s", (const char *) path);
        return false;
    }

    AAssetDir_rewind(dir);

    const char *fileName;
    while ((fileName = AAssetDir_getNextFileName(dir))) {
        result.push_back(nsFilePath(fileName));
    }

    AAssetDir_close(dir);
    return true;
}

void AndroidPlatform::SetScreenSize(int w, int h) {
    _screenWidth = w;
    _screenHeight = h;
}

bool AndroidPlatform::IsMobile() {
    return true;
}

bool AndroidPlatform::ShowInterstitialAd() {
    if (!_activity) {
        Log::Warning("Activity object is not initialized for ads!");
        return false;
    }

    auto method = _activity->BeginMethod("showInterstitialAd", "()Z");
    if (method) {
        auto res = _activity->GetEnv()->CallBooleanMethod(_activity->GetObject(), method);
        _activity->EndMethod();
        return res;
    }

    Log::Warning("Failed to call ads!");
    return false;
}

void AndroidPlatform::MessagePopup(const char *caption, const char *message) {
    //TODO: android message popup
}

IDataWriter *AndroidPlatform::InternalWrite(const char *fileName) {
    std::string filePath = _internalDataPath + "/" + fileName;
    return new nsFileWriter(filePath.c_str());
}

IDataReader *AndroidPlatform::InternalRead(const char *fileName) {
    std::string filePath = _internalDataPath + "/" + fileName;
    Log::Info("...reading internal: %s", filePath.c_str());
    return new nsFileReader(filePath.c_str());
}

ISoftInput *AndroidPlatform::GetSoftInput() {
    return _softInput;
}

void AndroidPlatform::OpenUrl(const char *url) {
    if (!_activity) {
        Log::Warning("Activity object is not initialized!");
        return;
    }

    auto method = _activity->BeginMethod("openUrl", "(Ljava/lang/String;)V");
    if (method) {
        auto env = _activity->GetEnv();
        auto javaUrl = env->NewStringUTF(url);
        env->CallVoidMethod(_activity->GetObject(), method, javaUrl);
        env->DeleteLocalRef(javaUrl);
        _activity->EndMethod();
        return;
    }

    Log::Warning("Failed to open url!");
}

const char *AndroidPlatform::GetDomainName() {
    return "Android";
}


