// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
#include "IntVar.h"

#include "Core/Parser.h"

bool nsIntVar::Serialize( nsScriptSaver &saver ) {
    saver.VarValue( "%d", GetValue() );
    return true;
}

bool nsIntVar::Deserialize( script_state_t *state ) {
    SetValue( static_cast<int>( ps_var_f( state ) ) );
    return true;
}
