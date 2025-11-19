//
// Created by Roman on 11/19/2025.
//

#include "BoolVar.h"

#include "ParserUtils.h"

bool nsBoolVar::Serialize(nsScriptSaver &ss) {
    ss.Printf("$%s %i", GetName(), GetValue());
    return true;
}

bool nsBoolVar::Deserialize(script_state_t *ss) {
    SetValue(ParseBool(ss, GetName(), GetValue()));
    return true;
}
