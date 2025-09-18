//
// Created by Roman on 3/26/2024.
//

#include "DesktopPlatform.h"
#include "nsLib/log.h"
#include "Core/sys.h"
#include "Core/FileReader.h"
#include "Core/FileWriter.h"
#include "Engine/input/SoftInputEmpty.h"
#include "Engine/input/soft/SoftInputKeyboard.h"
#include "Core/Config.h"
#include "Core/time/Time.h"
#include "env/Env.h"

bool DesktopPlatform::Init() {
    Log::Info("##### Init Desktop #####");

    _softInputChanged = [this](const nsBaseEvent*) -> void {
        UpdateSoftInput();
    };
    _useSoftInput = g_cfg->RegVar("si_force_soft", "0", GVF_SAVABLE);
    _useSoftInput->AddHandler(nsVar::NSVAR_CHANGED, _softInputChanged);
    UpdateSoftInput();

    if (!glfwInit()) {
        Sys_FatalError("Failed to init GLFW!\n");
        return false;
    }

    _wnd = nsEnv::Shared()->CreateGameWindow();
    if (!_wnd) {
        Sys_FatalError("Could not create game window!");
        return false;
    }
    glfwMakeContextCurrent(_wnd);

    _display = glfwGetPrimaryMonitor();

    GLint stencilBits = 0;
    glGetIntegerv(GL_STENCIL_BITS, &stencilBits);
    Log::Info("stencil bits: %i", stencilBits);

    return true;
}

void DesktopPlatform::UpdateSoftInput() {
    if (_useSoftInput->Bool() || IsMobile()) {
        _softInput = new nsSoftInputKeyboard();
    } else {
        _softInput = new SoftInputEmpty();
    }
}

void DesktopPlatform::Release() {
    Log::Info("##### Release Desktop #####");

    if (_wnd) {
        Log::Info("...destroy window");
        glfwDestroyWindow(_wnd);
        _wnd = nullptr;
    }
    _softInput = nullptr;
    glfwTerminate();
}

const char *DesktopPlatform::GetKeyName(int key) {
    return _keyNames.GetName(key);
}

unsigned int DesktopPlatform::GetTime() {
    return nsTime::GetTimeMS();
}

void DesktopPlatform::Minimize() {
    glfwIconifyWindow(_wnd);
}

void DesktopPlatform::SetAppTitle(const char *text) {
    if (text) {
        glfwSetWindowTitle(_wnd, text);
    }
}

void DesktopPlatform::GetClientSize(int &width, int &height) {
    nsEnv::Shared()->GetClientSize(width, height);
}

bool DesktopPlatform::IsKeyPressed(int key) {
    return glfwGetKey(_wnd, key) == GLFW_PRESS;
}

void DesktopPlatform::GetCursorPos(int &x, int &y) {
    double mouseX, mouseY;
    glfwGetCursorPos(_wnd, &mouseX, &mouseY);
    x = (int) mouseX;
    y = (int) mouseY;
}

void DesktopPlatform::ShowCursor(bool show) {
    nsEnv::Shared()->ShowCursor(show);
}

void DesktopPlatform::SetCursorPos(int x, int y) {
    glfwSetCursorPos(_wnd, x, y);
}

void *DesktopPlatform::GetWindowHandler() {
    return _wnd;
}

void DesktopPlatform::GetDisplayModes(Platform::tDisplayModesList &result) {
    int count;
    auto modes = glfwGetVideoModes(_display, &count);
    if (count < 1) {
        LogPrintf(PRN_ERROR, "Display modes are not available!");
    }

    LogPrintf(PRN_ALL, "num display modes: %i\n", count);

    for (int i = 0; i < count; ++i) {
        auto m = &modes[i];
        auto bits = m->redBits + m->greenBits + m->blueBits;

        DisplayMode mode = {
                m->width,
                m->height,
                m->refreshRate,
                bits
        };

        result.push_back(mode);

        LogPrintf(PRN_ALL, "Mode bpp:%i, %i x %i, format: %i%i%i, refresh: %i\n",
                  bits, m->width, m->height, m->redBits, m->greenBits, m->blueBits, m->refreshRate);
    }
}

bool DesktopPlatform::ApplyDisplayMode(int width, int height, bool fullScreen, int bits, int frequency) {
#if !defined(WEB_ASM)
    glfwGetError(nullptr);

    LogPrintf(PRN_ALL, "Applying display mode: %ix%i...\n", width, height);

    if (fullScreen) {
        glfwSetWindowMonitor(_wnd, _display, 0, 0, width, height, frequency);
    } else {
        glfwSetWindowMonitor(_wnd, nullptr, 0, 0, width, height,0);

        int count;
        auto monitors = glfwGetMonitors(&count);
        auto mode = glfwGetVideoMode(monitors[0]);
        if (mode) {
            int posX = (mode->width - width) / 2;
            int posY = (mode->height - height) / 2;
            glfwSetWindowPos(_wnd, posX, posY);
        } else {
            glfwSetWindowPos(_wnd, 50, 50);
        }

        float sx, sy;
        glfwGetMonitorContentScale(monitors[0], &sx, &sy);
        Log::Info("Monitor content scale: %fx%f", sx, sy);
    }

    if (glfwGetError(nullptr) != GLFW_NO_ERROR) {
        Sys_Message("Can't apply display mode: %ix%i", width, height);
        return false;
    }


    //glfwSwapInterval(r_vsync->Bool() ? 1 : 0);
    LogPrintf(PRN_ALL, "DONE\n");
#endif
    return true;
}

IDataReader *DesktopPlatform::FileRead(const char *fileName) {
    return new nsFileReader(fileName, "rb");
}

bool DesktopPlatform::FolderListing(const nsFilePath &folder, nsFilePath::tList &result) {
    return folder.Listing(result);
}

bool DesktopPlatform::IsMobile() {
    return nsEnv::Shared()->IsMobile();
}

Platform::ProcAddr DesktopPlatform::GetProcAddr(const char *name) {
    return glfwGetProcAddress(name);
}

void DesktopPlatform::SwapBuffers() {
    if (_wnd) {
        glfwSwapBuffers(_wnd);
    } else {
        Log::Warning("Can't swap buffers! Window is not initialized!");
    }
}

bool DesktopPlatform::ShowInterstitialAd() {
    Log::Warning("Ad is not implemented on Desktop!");

    return false;
}

void DesktopPlatform::MessagePopup(const char *caption, const char *message) {
    nsEnv::Shared()->MessagePopup(caption, message);
}

IDataWriter *DesktopPlatform::InternalWrite(const char *fileName) {
    return new nsFileWriter(fileName);
}

IDataReader *DesktopPlatform::InternalRead(const char *fileName) {
    return new nsFileReader(fileName);
}

ISoftInput *DesktopPlatform::GetSoftInput() {
    return _softInput;
}

void DesktopPlatform::OpenUrl(const char *url) {
    nsEnv::Shared()->OpenUrl(url);
}

const char * DesktopPlatform::GetDomainName() {
    return nsEnv::Shared()->GetDomainName();
}
