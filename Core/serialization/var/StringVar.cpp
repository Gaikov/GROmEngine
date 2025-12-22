//
// Created by Roman on 11/19/2025.
//

#include "StringVar.h"
#include "ParserUtils.h"

bool nsStringVar::Serialize(nsScriptSaver &ss) {
    ss.VarValue("\"%s\"",GetValue().c_str());
    return true;
}

bool nsStringVar::Deserialize(script_state_t *ss) {
    if (const auto value = ps_var_str(ss)) {
        SetValue(value);
    }
    return true;
}