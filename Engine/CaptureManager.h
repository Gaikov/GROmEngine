#pragma once

#include "nsLib/SubSystem.h"

class nsFilePath;
class nsVar;

class nsCaptureManager final : public nsSubSystem<nsCaptureManager> {
public:
    void Update();
    void ProcessCapture();

protected:
    bool OnInit() override;

private:
    static void CaptureScreen_f(int argc, const char *argv[]);

    void RequestCapture();
    bool GetNextCapturePath(nsFilePath &result) const;

    nsVar *_captureInterval = nullptr;
    unsigned int _lastUpdateTime = 0;
    float _elapsedSeconds = 0;
    float _trackedInterval = 0;
    bool _captureRequested = false;
};
