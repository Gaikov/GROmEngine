//
// Created by Roman on 4/2/2024.
//

#ifndef _GROM_ANDROIDPLATFORM_H
#define _GROM_ANDROIDPLATFORM_H

#include "engine/Platform.h"
#include "AndroidKeyboard.h"
#include "ImageDecoder/ImagesDecoder.h"
#include "GLNativeContext.h"
#include "AndroidCommon.h"
#include "JavaObject.h"

class AndroidPlatform : public Platform {
public:
    JavaObject::sp_t _activity;

public:
    static bool Create(JNIEnv *jenv, JavaVM *vm, AAssetManager *assetManager,
                       const char *internalDataPath);
    static void Destroy();

    void SetGLContext(GLNativeContext *context) { _glContext = context; }

public:
    bool Init() override;

    void Release() override;

    IDataReader *FileRead(const char *fileName) override;

    const nsArgs &GetArgs() override;

    void GetCursorPos(int &x, int &y) override;

    void SetCursorPos(int x, int y) override;

    void ShowCursor(bool show) override;

    bool IsKeyPressed(int key) override;

    const char *GetKeyName(int key) override;

    unsigned int GetTime() override;

    bool ShowInterstitialAd() override;

    void MessagePopup(const char *caption, const char *message) override;

    void Minimize() override;

    void SetAppTitle(const char *text) override;

    void GetClientSize(int &width, int &height) override;

    void *GetWindowHandler() override;

    void GetDisplayModes(tDisplayModesList &modes) override;

    bool ApplyDisplayMode(int width, int height, bool fullScreen, int bits, int frequency) override;

    bool FolderListing(const nsFilePath &folder, nsFilePath::tList &result) override;

    AndroidKeyboard* GetKeyboard() { return &_keyboard; }

    void SetScreenSize(int w, int h);

    bool IsMobile() override;

    void SwapBuffers() override;

    IDataWriter *InternalWrite(const char *fileName) override;

    IDataReader *InternalRead(const char *fileName) override;


    ISoftInput *GetSoftInput() override;
    void SetSoftInput(ISoftInput *si);


private:
    AndroidPlatform();

private:
    ISoftInput::sp_t _softInput;
    AAssetManager *_assetManager;
    GLNativeContext *_glContext;
    AndroidKeyboard _keyboard;
    ImagesDecoder _imagesDecoder;
    std::string _internalDataPath;
    int _screenWidth;
    int _screenHeight;
};


#endif //_GROM_ANDROIDPLATFORM_H
