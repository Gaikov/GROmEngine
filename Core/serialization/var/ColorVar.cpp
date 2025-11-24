//
// Created by Roman on 11/24/2025.
//

#include "ColorVar.h"

#include "Core/ParserUtils.h"

bool nsColorVar::Serialize(nsScriptSaver &ss) {
    const auto &v = GetValue();
    ss.Printf("$%s %f %f %f %f", GetName(), v.r, v.g, v.b, v.a);
    return false;
}

bool nsColorVar::Deserialize(script_state_t *ss) {
    nsColor c = GetValue();
    if (ParseColorExt(ss, GetName(), c)) {
        SetValue(c);
    }
    return true;
}
