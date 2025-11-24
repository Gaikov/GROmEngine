//
// Created by Roman on 11/24/2025.
//

#include "FloatVar.h"
#include "ParserUtils.h"

bool nsFloatVar::Serialize(nsScriptSaver &ss) {
    ss.Printf("$%s %f", GetName(), GetValue());
    return true;
}

bool nsFloatVar::Deserialize(script_state_t *ss) {
    SetValue(ParseFloat(ss, GetName(), GetValue()));
    return true;
}
