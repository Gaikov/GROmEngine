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
#include <memory>

class DesktopPlatform final : public Platform {
public:
    bool Init() override;
    void Release() override;

    bool IsMobile() override;

    const char *GetKeyName(int key) override;

    bool IsKeyPressed(int key) override;
    IGamepads* GetGamepadsProvider() override { return _gamepads.get(); }

    void SetCursorPos(int x, int y) override;

    void ShowCursor(bool show) override;

    void GetCursorPos(int &x, int &y) override;

    unsigned int GetTime() override;

    const nsArgs &GetArgs() override;

    bool ShowInterstitialAd() override;

    void MessagePopup(const char *caption, const char *message) override;

    void Minimize() override;

    void SetAppTitle(const char *text) override;

    void GetClientSize(int &width, int &height) override;

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

    nsFilePath GetInternalPath(const char *fileName) override;

    nsFilePath GetUserFolderPath() const override;

    void ConfigureInternalStorage(const nsFilePath &root) override;

    ISoftInput *GetSoftInput() override;

    void OpenUrl(const char *url) override;

    const char * GetDomainName() override;

private:
    static constexpr int WindowChannelBits = 8;

    struct DisplayState {
        GLFWmonitor *monitor = nullptr;
        DisplayMode mode{};
        int x = 0, y = 0;

        bool operator==(const DisplayState &other) const {
            return monitor == other.monitor && mode.width == other.mode.width && mode.height == other.mode.height
                && mode.frequency == other.mode.frequency && mode.bits == other.mode.bits
                && x == other.x && y == other.y;
        }
    };

    DesktopKeyNames _keyNames;
    std::unique_ptr<IGamepads> _gamepads;

    GLFWwindow  *_wnd = nullptr;
    GLFWmonitor *_display = nullptr;
    int _desktopFrequency = 0;
    bool _firstDisplayMode = true;
    ISoftInput::sp_t _softInput;
    nsVar   *_useSoftInput = nullptr;
    nsFilePath _internalRoot = "";

    void UpdateSoftInput();
    DisplayState GetDisplayState() const;
    void RestoreDisplayState(const DisplayState &previous);
    bool ApplyWindowMode(int width, int height);
    bool ApplyFullscreenMode(int width, int height, int bits, int frequency);
    static DisplayMode ReadDisplayMode(GLFWmonitor *monitor);
    static int GetDisplayColorDistance(const GLFWvidmode &mode);
};
