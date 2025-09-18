//
// Created by Roman on 3/26/2024.
//

#pragma once


#include "DesktopCommon.h"
#include "Engine/Platform.h"
#include "DesktopKeyNames.h"
#include "Core/DataWriter.h"
#include "nsLib/FilePath.h"
#include "Core/Var.h"

class DesktopPlatform final : public Platform {
public:
    bool Init() override;
    void Release() override;

    bool IsMobile() override;

    const char *GetKeyName(int key) override;

    bool IsKeyPressed(int key) override;

    void SetCursorPos(int x, int y) override;

    void ShowCursor(bool show) override;

    void GetCursorPos(int &x, int &y) override;

    unsigned int GetTime() override;

    const nsArgs &GetArgs() override;

    bool ShowInterstitialAd() override;

    void MessagePopup(const char *caption, const char *message) override;

    void Minimize() override;

    void SetAppTitle(const char *text) override;

    void GetClientSize(int &width, int &eight) override;

    void GetDisplayModes(tDisplayModesList &modes) override;

    bool ApplyDisplayMode(int width, int height, bool fullScreen, int bits, int frequency) override;

    void *GetWindowHandler() override;

    bool FolderListing(const nsFilePath &folder, nsFilePath::tList &result) override;

    IDataReader *FileRead(const char *fileName) override;

    GLFWwindow* GetWindow() { return _wnd; }

    ProcAddr GetProcAddr(const char *name) override;

    void SwapBuffers() override;

    IDataWriter *InternalWrite(const char *fileName) override;

    IDataReader *InternalRead(const char *fileName) override;

    ISoftInput *GetSoftInput() override;

    void OpenUrl(const char *url) override;

    const char * GetDomainName() override;

private:
    DesktopKeyNames _keyNames;

    GLFWwindow  *_wnd = nullptr;
    GLFWmonitor *_display = nullptr;
    ISoftInput::sp_t _softInput;
    nsVar   *_useSoftInput = nullptr;
    nsEventDispatcher::tEventHandler _softInputChanged;

    void UpdateSoftInput();
};