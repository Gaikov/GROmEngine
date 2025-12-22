//
// Created by Roman on 11/24/2025.
//

#include "ColorVar.h"

#include "Core/ParserUtils.h"

bool nsColorVar::Serialize(nsScriptSaver &ss) {
    const auto &v = GetValue();
    ss.VarValue("%f %f %f %f",  v.r, v.g, v.b, v.a);
    return true;
}

bool nsColorVar::Deserialize(script_state_t *ss) {
    nsColor c = GetValue();
    if (ParseColorExt(ss, c)) {
        SetValue(c);
    }
    return true;
}
