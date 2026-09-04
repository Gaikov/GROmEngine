#include <string>

#include <GLFW/glfw3.h>

#include "Engine/Platform.h"
#include "Share/ShareBackend.h"

namespace {
std::string BuildClipboardText(const nsShareRequest &request) {
    auto value = request.text;
    if (!value.empty() && !request.url.empty()) value += '\n';
    value += request.url;
    return value;
}

class DesktopClipboardShareBackend final : public nsShareService::IBackend {
public:
    using RequestId = nsShareService::RequestId;

    bool Share(RequestId requestId, const nsShareRequest &request) override {
        const auto value = BuildClipboardText(request);
        if (value.empty()) return false;
        auto *window = static_cast<GLFWwindow *>(App_GetPlatform()->GetWindowHandler());
        glfwSetClipboardString(window, value.c_str());
        nsShareService::Complete(requestId, {nsShareStatus::Copied, {}});
        return true;
    }

    void CancelAll() override {
    }
};
}

nsShareService::IBackend *CreateShareBackend() {
    return new DesktopClipboardShareBackend();
}
