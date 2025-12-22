//
// Created by Roman on 11/24/2025.
//

#include "FloatVar.h"
#include "ParserUtils.h"

bool nsFloatVar::Serialize(nsScriptSaver &ss) {
    ss.VarValue("%f", GetValue());
    return true;
}

bool nsFloatVar::Deserialize(script_state_t *ss) {
    SetValue(ps_var_f(ss));
    return true;
}
