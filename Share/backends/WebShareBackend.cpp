#include <emscripten/emscripten.h>

#include "Share/ShareBackend.h"

extern "C" EMSCRIPTEN_KEEPALIVE void nsShareWebComplete(
        double requestId, int status, const char *diagnostic) {
    nsShareResult result;
    result.status = static_cast<nsShareStatus>(status);
    if (diagnostic) result.diagnostic = diagnostic;
    nsShareService::Complete(
            static_cast<nsShareService::RequestId>(requestId), std::move(result));
}

EM_JS(void, StartWebShare, (double requestId, const char *titleValue,
        const char *textValue, const char *urlValue, const char *imagePathValue), {
    const title = UTF8ToString(titleValue);
    const text = UTF8ToString(textValue);
    const url = UTF8ToString(urlValue);
    const imagePath = UTF8ToString(imagePathValue);

    const complete = (status, diagnostic) => {
        const ptr = diagnostic ? stringToNewUTF8(String(diagnostic)) : 0;
        _nsShareWebComplete(requestId, status, ptr);
        if (ptr) _free(ptr);
    };

    const copyToClipboard = async () => {
        const value = text && url ? text + '\n' + url : text + url;
        try {
            if (navigator.clipboard && window.isSecureContext) {
                await navigator.clipboard.writeText(value);
            } else {
                const area = document.createElement('textarea');
                area.value = value;
                area.setAttribute('readonly', 'readonly');
                area.style.position = 'fixed';
                area.style.opacity = '0';
                document.body.appendChild(area);
                area.select();
                const copied = document.execCommand('copy');
                document.body.removeChild(area);
                if (!copied) throw new Error('Clipboard is unavailable');
            }
            complete(2, null);
        } catch (error) {
            complete(5, error);
        }
    };

    if (!navigator.share) {
        copyToClipboard();
        return;
    }

    const shareData = {title, text, url};
    if (imagePath) {
        try {
            const path = imagePath.startsWith('/') ? imagePath : '/' + imagePath;
            const bytes = FS.readFile(path);
            const file = new File([bytes], 'share.png', {type: 'image/png'});
            if (navigator.canShare && navigator.canShare({files: [file]})) {
                shareData.files = [file];
            }
        } catch (error) {
            console.warn('Share image is unavailable; sharing text and link only', error);
        }
    }

    navigator.share(shareData).then(() => {
        complete(1, null);
    }).catch(error => {
        if (error && error.name === 'AbortError') complete(3, null);
        else complete(5, error);
    });
});

namespace {
class WebShareBackend final : public nsShareService::IBackend {
public:
    using RequestId = nsShareService::RequestId;

    bool Share(RequestId requestId, const nsShareRequest &request) override {
        StartWebShare(static_cast<double>(requestId), request.title.c_str(),
                request.text.c_str(), request.url.c_str(), request.imageAssetPath.c_str());
        return true;
    }

    void CancelAll() override {
    }
};
}

nsShareService::IBackend *CreateShareBackend() {
    return new WebShareBackend();
}
