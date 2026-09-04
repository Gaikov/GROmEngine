#include <filesystem>
#include <string>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "Engine/Platform.h"
#include "Share/ShareBackend.h"

#if defined(_MSC_VER)
#include <windows.h>
#include <windows.applicationmodel.datatransfer.interop.h>
#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/base.h>
#endif

namespace {
std::string BuildClipboardText(const nsShareRequest &request) {
    auto value = request.text;
    if (!value.empty() && !request.url.empty()) value += '\n';
    value += request.url;
    return value;
}

class WindowsShareBackend final : public nsShareService::IBackend {
public:
    using RequestId = nsShareService::RequestId;

#if defined(_MSC_VER)
    ~WindowsShareBackend() override {
        if (_manager && _dataRequestedToken.value) _manager.DataRequested(_dataRequestedToken);
    }
#endif

    bool Share(RequestId requestId, const nsShareRequest &request) override {
#if defined(_MSC_VER)
        try {
            auto *glfwWindow = static_cast<GLFWwindow *>(App_GetPlatform()->GetWindowHandler());
            const auto hwnd = glfwWindow ? glfwGetWin32Window(glfwWindow) : nullptr;
            if (!hwnd) return Copy(requestId, request);

            winrt::init_apartment(winrt::apartment_type::single_threaded);
            auto interop = winrt::get_activation_factory<
                    winrt::Windows::ApplicationModel::DataTransfer::DataTransferManager,
                    IDataTransferManagerInterop>();
            winrt::check_hresult(interop->GetForWindow(
                    hwnd,
                    winrt::guid_of<winrt::Windows::ApplicationModel::DataTransfer::DataTransferManager>(),
                    winrt::put_abi(_manager)));

            if (_dataRequestedToken.value) _manager.DataRequested(_dataRequestedToken);
            _request = request;
            _dataRequestedToken = _manager.DataRequested(
                    [this](auto const &, auto const &args) { FillRequest(args); });
            winrt::check_hresult(interop->ShowShareUIForWindow(hwnd));
            nsShareService::Complete(requestId, {nsShareStatus::Presented, {}});
            return true;
        } catch (...) {
            return Copy(requestId, request);
        }
#else
        return Copy(requestId, request);
#endif
    }

    void CancelAll() override {
    }

private:
    bool Copy(RequestId requestId, const nsShareRequest &request) {
        const auto value = BuildClipboardText(request);
        if (value.empty()) return false;
        auto *window = static_cast<GLFWwindow *>(App_GetPlatform()->GetWindowHandler());
        glfwSetClipboardString(window, value.c_str());
        nsShareService::Complete(requestId, {nsShareStatus::Copied, {}});
        return true;
    }

#if defined(_MSC_VER)
    void FillRequest(const winrt::Windows::ApplicationModel::DataTransfer::DataRequestedEventArgs &args) {
        using namespace winrt::Windows::ApplicationModel::DataTransfer;
        const auto data = args.Request().Data();
        data.Properties().Title(winrt::to_hstring(_request.title));
        if (!_request.text.empty()) data.SetText(winrt::to_hstring(_request.text));
        if (!_request.url.empty()) {
            data.SetWebLink(winrt::Windows::Foundation::Uri(winrt::to_hstring(_request.url)));
        }

        if (!_request.imageAssetPath.empty()) {
            try {
                const auto path = std::filesystem::absolute(_request.imageAssetPath).wstring();
                const auto file = winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(path).get();
                data.SetBitmap(winrt::Windows::Storage::Streams::RandomAccessStreamReference::CreateFromFile(file));
            } catch (...) {
                // Text and URL are still valid share content.
            }
        }
    }

    nsShareRequest _request;
    winrt::Windows::ApplicationModel::DataTransfer::DataTransferManager _manager {nullptr};
    winrt::event_token _dataRequestedToken {};
#endif
};
}

nsShareService::IBackend *CreateShareBackend() {
    return new WindowsShareBackend();
}
