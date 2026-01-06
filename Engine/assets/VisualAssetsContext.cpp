//
// Created by Roman on 1/6/2026.
//

#include "VisualAssetsContext.h"
#include "Core/ParserUtils.h"
#include "nsLib/log.h"

nsVisualAssetsContext::nsVisualAssetsContext() : assetsPath("") {
}

nsVisualAssetsContext::~nsVisualAssetsContext() {
    Log::Info("...VisualAssetsContext destroyed");
}

nsFilePath nsVisualAssetsContext::ParseAssetPath(script_state_t *ss, const char *name) const {
    const nsString path = ParseStrP(ss, name, "");
    if (!path.IsEmpty()) {
        return assetsPath.ResolvePath(path);
    }
    return {path};
}

void nsVisualAssetsContext::SaveAssetPath(const nsScriptSaver &saver, const char *name, const nsFilePath &path) const {
    saver.VarString(name, assetsPath.GetRelativePath(path));
}

nsString nsVisualAssetsContext::RelativeAssetPath(const nsFilePath &path) const {
    return assetsPath.GetRelativePath(path);
}