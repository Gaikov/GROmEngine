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

#include <algorithm>
#include <cstdlib>
#include <filesystem>

bool DesktopPlatform::Init() {
    Log::Info("##### Init Desktop #####");

    _useSoftInput = g_cfg->RegVar("si_force_soft", "0", GVF_SAVABLE);
    _useSoftInput->AddHandler(nsVar::NSVAR_CHANGED, [this](const nsBaseEvent*) -> void {
        UpdateSoftInput();
    });
    UpdateSoftInput();

#ifdef __APPLE__
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif

    if (!glfwInit()) {
        Sys_FatalError("Failed to init GLFW!\n");
        return false;
    }

#ifndef __APPLE__
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

#ifdef WEB_ASM
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
#endif

    _wnd = nsEnv::Shared()->CreateGameWindow();
    if (!_wnd) {
        Sys_FatalError("Could not create game window!");
        return false;
    }
#ifndef __APPLE__
    glfwMakeContextCurrent(_wnd);
#endif

    _display = glfwGetPrimaryMonitor();
    _firstDisplayMode = true;

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
#ifdef WEB_ASM
    return nsEnv::IsKeyPressed(key);
#else
    return glfwGetKey(_wnd, key) == GLFW_PRESS;
#endif
}

void DesktopPlatform::GetCursorPos(int &x, int &y) {
    double mouseX, mouseY;
    glfwGetCursorPos(_wnd, &mouseX, &mouseY);
    nsEnv::Shared()->WindowToClientCoordinates(mouseX, mouseY);
    x = (int) mouseX;
    y = (int) mouseY;
}

void DesktopPlatform::ShowCursor(bool show) {
    nsEnv::Shared()->ShowCursor(show);
}

void DesktopPlatform::SetCursorPos(int x, int y) {
    double windowX = x;
    double windowY = y;
    nsEnv::Shared()->ClientToWindowCoordinates(windowX, windowY);
    glfwSetCursorPos(_wnd, windowX, windowY);
}

void *DesktopPlatform::GetWindowHandler() {
    return _wnd;
}

void DesktopPlatform::GetDisplayModes(Platform::tDisplayModesList &result) {
    int count;
    auto modes = glfwGetVideoModes(_display, &count);
    if (count < 1) {
        Log::Error("Display modes are not available!");
    }

    Log::Info("num display modes: %i", count);

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

        Log::Info("Mode bpp:%i, %i x %i, format: %i%i%i, refresh: %i",
                  bits, m->width, m->height, m->redBits, m->greenBits, m->blueBits, m->refreshRate);
    }
}

#if !defined(WEB_ASM)
static bool FitStartupWindow(GLFWwindow *window, GLFWmonitor *display, int &width, int &height, int &x, int &y) {
    if (!display || width <= 0 || height <= 0) return false;

    int availableWidth, availableHeight;
    glfwGetMonitorWorkarea(display, &x, &y, &availableWidth, &availableHeight);
    if (availableWidth <= 0 || availableHeight <= 0) {
        const auto *mode = glfwGetVideoMode(display);
        if (!mode) return false;
        glfwGetMonitorPos(display, &x, &y);
        availableWidth = mode->width;
        availableHeight = mode->height;
    }

    int left, top, right, bottom;
    glfwGetWindowFrameSize(window, &left, &top, &right, &bottom);
    availableWidth -= left + right;
    availableHeight -= top + bottom;
    if (availableWidth <= 0 || availableHeight <= 0
        || (width <= availableWidth && height <= availableHeight)) return false;

    const auto scale = std::min(static_cast<double>(availableWidth) / width,
                                static_cast<double>(availableHeight) / height);
    width = std::clamp(static_cast<int>(width * scale), 1, availableWidth);
    height = std::clamp(static_cast<int>(height * scale), 1, availableHeight);
    x += left + (availableWidth - width) / 2;
    y += top + (availableHeight - height) / 2;
    return true;
}
#endif

bool DesktopPlatform::ApplyDisplayMode(int width, int height, bool fullScreen, int bits, int frequency) {
#if !defined(WEB_ASM)
    int startupX = 0;
    int startupY = 0;
    const auto fitted = _firstDisplayMode && !fullScreen
        && FitStartupWindow(_wnd, _display, width, height, startupX, startupY);
    glfwGetError(nullptr);

    Log::Info("Applying display mode: %ix%i...", width, height);

    if (fullScreen) {
        glfwSetWindowMonitor(_wnd, _display, 0, 0, width, height, frequency);
    } else {
        glfwSetWindowMonitor(_wnd, nullptr, 0, 0, width, height,0);

        if (_display) {
            if (fitted) {
                glfwSetWindowPos(_wnd, startupX, startupY);
            } else if (const auto *mode = glfwGetVideoMode(_display)) {
                int posX = (mode->width - width) / 2;
                int posY = (mode->height - height) / 2;
                glfwSetWindowPos(_wnd, posX, posY);
            } else {
                glfwSetWindowPos(_wnd, 50, 50);
            }

            float sx, sy;
            glfwGetMonitorContentScale(_display, &sx, &sy);
            Log::Info("Monitor content scale: %fx%f", sx, sy);
        }
    }

    if (glfwGetError(nullptr) != GLFW_NO_ERROR) {
        Sys_Message("Can't apply display mode: %ix%i", width, height);
        return false;
    }
    _firstDisplayMode = false;


    //glfwSwapInterval(r_vsync->Bool() ? 1 : 0);
    Log::Info("DONE");
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
#ifdef __APPLE__
    // Metal handles presentation via presentDrawable in EndScene
#else
    if (_wnd) {
        glfwSwapBuffers(_wnd);
    } else {
        Log::Warning("Can't swap buffers! Window is not initialized!");
    }
#endif
}

bool DesktopPlatform::ShowInterstitialAd() {
    Log::Warning("Ad is not implemented on Desktop!");

    return false;
}

void DesktopPlatform::MessagePopup(const char *caption, const char *message) {
    nsEnv::Shared()->MessagePopup(caption, message);
}

IDataWriter *DesktopPlatform::InternalWrite(const char *fileName) {
    const auto path = GetInternalPath(fileName);
    if (path.IsEmpty() || !path.CreateParentFolders()) return new nsFileWriter(nullptr);
    return new nsFileWriter(path.AsChar());
}

IDataReader *DesktopPlatform::InternalRead(const char *fileName) {
    const auto path = GetInternalPath(fileName);
    return new nsFileReader(path.IsEmpty() ? nullptr : path.AsChar());
}

nsFilePath DesktopPlatform::GetInternalPath(const char *fileName) {
    if (!fileName || !*fileName) return nsFilePath("");
#ifdef WEB_ASM
    const auto root = _internalRoot.IsEmpty() ? nsFilePath("/internal") : _internalRoot;
#else
    if (_internalRoot.IsEmpty()) return nsFilePath(fileName);
    const auto &root = _internalRoot;
#endif
    const nsFilePath normalizedName(fileName);
    if (nsFilePath::IsAbsolute(normalizedName.AsChar())) return nsFilePath("");
    const auto rawRelative = std::filesystem::path(normalizedName.AsChar());
    if (rawRelative.has_root_path()) return nsFilePath("");
    for (const auto &part : rawRelative) {
        if (part == "..") return nsFilePath("");
    }
    const auto relative = rawRelative.lexically_normal();
    if (relative.empty()) return nsFilePath("");
    const auto path = root.ResolvePath(relative.generic_string().c_str());
    nsFilePath canonicalRoot("");
    nsFilePath canonicalPath("");
    if (!root.GetCanonical(canonicalRoot) || !path.GetCanonical(canonicalPath)
        || !canonicalPath.IsWithin(canonicalRoot,
#ifdef _WIN32
            true
#else
            false
#endif
        )) return nsFilePath("");
    return relative == "." ? root : path;
}

nsFilePath DesktopPlatform::GetUserFolderPath() const {
#ifdef WEB_ASM
    return nsFilePath(".");
#else
#ifdef _WIN32
    const auto *profile = std::getenv("USERPROFILE");
#else
    const auto *profile = std::getenv("HOME");
#endif
    return nsFilePath(profile ? profile : "");
#endif
}

void DesktopPlatform::ConfigureInternalStorage(const nsFilePath &root) {
    _internalRoot = root;
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
