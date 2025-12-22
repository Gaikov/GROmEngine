//
// Created by Roman on 11/19/2025.
//

#include "BoolVar.h"

#include "ParserUtils.h"

bool nsBoolVar::Serialize(nsScriptSaver &ss) {
    ss.VarValue("%i", GetValue());
    return true;
}

bool nsBoolVar::Deserialize(script_state_t *ss) {
    SetValue(ps_var_f(ss) != 0);
    return true;
}
