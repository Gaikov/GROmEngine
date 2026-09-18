//
// Created by Roman on 09/18/2026.
//

#include "Float2Var.h"

#include "Core/ParserUtils.h"

bool nsFloat2Var::Serialize( nsScriptSaver &ss )
{
    const auto &v = GetValue();
    ss.VarValue( "%f %f", v.x, v.y );
    return true;
}

bool nsFloat2Var::Deserialize( script_state_t *ss )
{
    float values[2] = { GetValue().x, GetValue().y };
    if ( ps_var_2f( ss, values ) )
    {
        SetValue( nsVec2( values[0], values[1] ) );
    }
    return true;
}
