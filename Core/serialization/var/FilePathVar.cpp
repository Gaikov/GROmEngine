//
// Created by Roman on 5/20/2026.
//

#include "FilePathVar.h"

#include "ParserUtils.h"

bool nsFilePathVar::Serialize(nsScriptSaver &ss) {
    std::string value = GetValue();
    if (!value.empty() && !_baseFolder.IsEmpty() && nsFilePath::IsAbsolute(value.c_str())) {
        value = _baseFolder.GetRelativePath(value.c_str()).AsChar();
    }

    ss.VarValue("\"%s\"", value.c_str());
    return true;
}

bool nsFilePathVar::Deserialize(script_state_t *ss) {
    if (const auto value = ps_var_str(ss)) {
        if (*value && !_baseFolder.IsEmpty() && !nsFilePath::IsAbsolute(value)) {
            SetValue(_baseFolder.ResolvePath(value).AsChar());
        } else {
            SetValue(value);
        }
    }
    return true;
}
