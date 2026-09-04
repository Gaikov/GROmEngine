#import <AppKit/AppKit.h>

#include <GLFW/glfw3.h>

#include "Engine/Platform.h"
#include "Share/ShareBackend.h"

namespace {
NSString *ToString(const std::string &value) {
    return [NSString stringWithUTF8String:value.c_str()];
}

std::string BuildClipboardText(const nsShareRequest &request) {
    auto value = request.text;
    if (!value.empty() && !request.url.empty()) value += '\n';
    value += request.url;
    return value;
}

class AppleShareBackend final : public nsShareService::IBackend {
public:
    using RequestId = nsShareService::RequestId;

    ~AppleShareBackend() override {
#if !__has_feature(objc_arc)
        [_picker release];
#endif
    }

    bool Share(RequestId requestId, const nsShareRequest &request) override {
        @autoreleasepool {
            auto *window = NSApp.keyWindow ?: NSApp.mainWindow;
            auto *view = window.contentView;
            if (!view) return Copy(requestId, request);

            auto *items = [NSMutableArray array];
            if (!request.text.empty()) [items addObject:ToString(request.text)];
            if (!request.url.empty()) {
                if (auto *url = [NSURL URLWithString:ToString(request.url)]) [items addObject:url];
            }
            if (!request.imageAssetPath.empty()) {
                auto *path = ToString(request.imageAssetPath);
                if (!path.isAbsolutePath) {
                    path = [NSFileManager.defaultManager.currentDirectoryPath
                            stringByAppendingPathComponent:path];
                }
                auto *image = [[NSImage alloc] initWithContentsOfFile:path];
                if (image) [items addObject:image];
#if !__has_feature(objc_arc)
                [image release];
#endif
            }
            if (!items.count) return false;

            auto *picker = [[NSSharingServicePicker alloc] initWithItems:items];
#if __has_feature(objc_arc)
            _picker = picker;
#else
            [_picker release];
            _picker = picker;
#endif
            [_picker showRelativeToRect:NSMakeRect(NSMidX(view.bounds), NSMidY(view.bounds), 1, 1)
                                  ofView:view
                           preferredEdge:NSRectEdgeMinY];
            nsShareService::Complete(requestId, {nsShareStatus::Presented, {}});
            return true;
        }
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

    __strong NSSharingServicePicker *_picker = nil;
};
}

nsShareService::IBackend *CreateShareBackend() {
    return new AppleShareBackend();
}
