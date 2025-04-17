//
// Created by Roman on 3/26/2024.
//

#ifndef _GROM_ENGINE_PLATFORM_H
#define _GROM_ENGINE_PLATFORM_H

#include "Core/Args.h"
#include "Core/DataWriter.h"
#include "nsLib/FilePath.h"
#include "input/ISoftInput.h"

class Platform {
public:
    struct DisplayMode {
        int width, height;
        int frequency;
        int bits;
    };

    typedef std::vector<DisplayMode>    tDisplayModesList;
    virtual ~Platform() = default;

    //App
    virtual bool Init() = 0;
    virtual void Release() = 0;
    virtual const nsArgs &GetArgs() = 0;
    virtual bool IsMobile() = 0;
    virtual bool ShowInterstitialAd() = 0;

    //Input
    virtual void GetCursorPos(int &x, int &y) = 0;
    virtual void SetCursorPos(int x, int y) = 0;
    virtual void ShowCursor(bool show) = 0;
    virtual bool IsKeyPressed(int key) = 0;
    virtual const char* GetKeyName(int key) = 0;
    virtual ISoftInput* GetSoftInput() = 0;

    //Time
    virtual unsigned int GetTime() = 0;

    //Window system
    virtual void MessagePopup(const char *caption, const char *message) = 0;
    virtual void Minimize() = 0;
    virtual void SetAppTitle(const char *text) = 0;
    virtual void GetClientSize(int &width, int &height) = 0;
    virtual void* GetWindowHandler() = 0;
    virtual void OpenUrl(const char *url) = 0;

    //Display modes
    virtual void GetDisplayModes(tDisplayModesList &modes) = 0;
    virtual bool ApplyDisplayMode(int width, int height, bool fullScreen, int bits, int frequency) = 0;

    //File system
    virtual IDataReader* FileRead(const char *fileName) = 0;
    virtual bool FolderListing(const nsFilePath &folder, nsFilePath::tList &result) = 0;
    virtual IDataWriter* InternalWrite(const char *fileName) = 0;
    virtual IDataReader* InternalRead(const char *fileName) = 0;

    //Renderer
    virtual void SwapBuffers() = 0;
};

Platform *App_GetPlatform();

#endif //_GROM_ENGINE_PLATFORM_H
