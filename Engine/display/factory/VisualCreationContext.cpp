//
// Created by Roman on 12/9/2025.
//
#include "VisualCreationContext.h"

#include "Core/ParserUtils.h"

nsVisualCreationContext2d::nsVisualCreationContext2d() : assetsPath("") {
}

nsFilePath nsVisualCreationContext2d::ParseAssetPath(script_state_t *ss, const char *name) const {
    const nsString path = ParseStrP(ss, name, "");
    if (!path.IsEmpty()) {
        return assetsPath.ResolvePath(path);
    }
    return nsFilePath(path);
}

void nsVisualCreationContext2d::SaveAssetPath(const nsScriptSaver &saver, const char *name, const nsFilePath &path) const {
    saver.VarString(name, assetsPath.GetRelativePath(path));
}

nsString nsVisualCreationContext2d::RelativeAssetPath(const nsFilePath &path) const {
    return assetsPath.GetRelativePath(path);
}
