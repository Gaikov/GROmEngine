#include "CaptureManager.h"

#include <charconv>
#include <cstdint>
#include <limits>
#include <string_view>

#include "Core/Config.h"
#include "ImageDecoder/ImagesDecoder.h"
#include "Platform.h"
#include "RenManager.h"
#include "nsLib/FilePath.h"
#include "nsLib/log.h"

namespace {
constexpr auto CaptureFolder = "screens";
constexpr std::string_view CapturePrefix = "capture";
constexpr std::string_view CaptureSuffix = ".png";

bool GetCaptureIndex(const nsFilePath &path, std::uint64_t &result) {
    if (path.IsFolder()) {
        return false;
    }

    auto fileName = path.GetName();
    const std::string_view name = fileName.AsChar();
    if (!name.starts_with(CapturePrefix) || !name.ends_with(CaptureSuffix)) {
        return false;
    }

    const auto digits = name.substr(CapturePrefix.size(),
                                    name.size() - CapturePrefix.size() - CaptureSuffix.size());
    if (digits.empty()) {
        return false;
    }

    std::uint64_t value = 0;
    const auto parseResult = std::from_chars(digits.data(), digits.data() + digits.size(), value);
    if (parseResult.ec != std::errc() || parseResult.ptr != digits.data() + digits.size()) {
        return false;
    }

    result = value;
    return true;
}
}

//---------------------------------------------------------
// nsCaptureManager::OnInit:
//---------------------------------------------------------
bool nsCaptureManager::OnInit() {
    g_cfg->RegCmd("capture_screen", CaptureScreen_f);
    _captureInterval = g_cfg->RegVar("capture_interval", "0", 0);
    _captureInterval->SetValue(0);
    _trackedInterval = _captureInterval->Value();
    _lastUpdateTime = App_GetPlatform()->GetTime();
    return true;
}

//---------------------------------------------------------
// nsCaptureManager::CaptureScreen_f:
//---------------------------------------------------------
void nsCaptureManager::CaptureScreen_f(int, const char *[]) {
    Shared()->RequestCapture();
}

//---------------------------------------------------------
// nsCaptureManager::RequestCapture:
//---------------------------------------------------------
void nsCaptureManager::RequestCapture() {
    if (_captureRequested) {
        Log::Warning("Screen capture is already pending");
        return;
    }

    _captureRequested = true;
    _elapsedSeconds = 0;
    Log::Info("Screen capture requested");
}

//---------------------------------------------------------
// nsCaptureManager::Update:
//---------------------------------------------------------
void nsCaptureManager::Update() {
    const auto currentTime = App_GetPlatform()->GetTime();
    const auto elapsedMs = currentTime - _lastUpdateTime;
    _lastUpdateTime = currentTime;

    const auto interval = _captureInterval->Value();
    if (interval <= 0) {
        _trackedInterval = interval;
        _elapsedSeconds = 0;
        return;
    }

    if (interval != _trackedInterval) {
        _trackedInterval = interval;
        _elapsedSeconds = 0;
        return;
    }

    _elapsedSeconds += static_cast<float>(elapsedMs) / 1000.0f;
    if (_elapsedSeconds < interval) {
        return;
    }

    _elapsedSeconds = 0;
    if (!_captureRequested) {
        _captureRequested = true;
        Log::Info("Automatic screen capture requested");
    }
}

//---------------------------------------------------------
// nsCaptureManager::GetNextCapturePath:
//---------------------------------------------------------
bool nsCaptureManager::GetNextCapturePath(nsFilePath &result) const {
    const nsFilePath folder(CaptureFolder);
    if (!folder.CreateFolders()) {
        Log::Error("Unable to create screen capture folder: '%s'", folder.AsChar());
        return false;
    }

    nsFilePath::tList files;
    if (!folder.Listing(files)) {
        Log::Error("Unable to list screen capture folder: '%s'", folder.AsChar());
        return false;
    }

    auto nextIndex = std::uint64_t(0);
    auto foundCapture = false;
    for (const auto &file : files) {
        std::uint64_t index = 0;
        if (!GetCaptureIndex(file, index)) {
            continue;
        }
        if (!foundCapture || index >= nextIndex) {
            if (index == std::numeric_limits<std::uint64_t>::max()) {
                Log::Error("Screen capture index overflow");
                return false;
            }
            nextIndex = index + 1;
            foundCapture = true;
        }
    }

    while (true) {
        nsString fileName;
        fileName.Format("capture%03llu.png", static_cast<unsigned long long>(nextIndex));
        result = folder.ResolvePath(fileName);
        if (!result.IsExists()) {
            return true;
        }
        if (nextIndex == std::numeric_limits<std::uint64_t>::max()) {
            Log::Error("Screen capture index overflow");
            return false;
        }
        ++nextIndex;
    }
}

//---------------------------------------------------------
// nsCaptureManager::ProcessCapture:
//---------------------------------------------------------
void nsCaptureManager::ProcessCapture() {
    if (!_captureRequested) {
        return;
    }
    _captureRequested = false;
    _elapsedSeconds = 0;

    nsFilePath path("");
    if (!GetNextCapturePath(path)) {
        return;
    }

    const auto bitmap = nsRenDevice::Shared()->Device()->CaptureScreen();
    if (!bitmap) {
        Log::Error("Unable to capture screen pixels");
        return;
    }

    const ImagesDecoder encoder;
    if (!encoder.Write(*bitmap, path.AsChar())) {
        Log::Error("Unable to save screen capture: '%s'", path.AsChar());
        return;
    }

    Log::Info("Screen capture saved: '%s'", path.AsChar());
}
