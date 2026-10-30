#pragma once

#include "nsLib/SubSystem.h"

class nsFilePath;

class nsCaptureManager final : public nsSubSystem<nsCaptureManager> {
public:
    void ProcessCapture();

protected:
    bool OnInit() override;

private:
    static void CaptureScreen_f(int argc, const char *argv[]);

    void RequestCapture();
    bool GetNextCapturePath(nsFilePath &result) const;

    bool _captureRequested = false;
};
