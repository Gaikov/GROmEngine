//
// Created by Roman on 11/19/2025.
//

#include "StringVar.h"
#include "ParserUtils.h"

bool nsStringVar::Serialize(nsScriptSaver &ss) {
    ss.Printf("$%s \"%s\"", GetName(), GetValue().c_str());
    return true;
}

bool nsStringVar::Deserialize(script_state_t *ss) {
    SetValue(ParseStrP(ss, GetName(), GetValue().c_str()));
    return true;
}
