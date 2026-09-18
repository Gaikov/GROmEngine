//
// Created by Roman on 09/18/2026.
//

#include "Float3Var.h"

#include "Core/ParserUtils.h"

bool nsFloat3Var::Serialize( nsScriptSaver &ss )
{
    const auto &v = GetValue();
    ss.VarValue( "%f %f %f", v.x, v.y, v.z );
    return true;
}

bool nsFloat3Var::Deserialize( script_state_t *ss )
{
    float values[3] = { GetValue().x, GetValue().y, GetValue().z };
    if ( ps_var_3f( ss, values ) )
    {
        SetValue( nsVec3( values[0], values[1], values[2] ) );
    }
    return true;
}
