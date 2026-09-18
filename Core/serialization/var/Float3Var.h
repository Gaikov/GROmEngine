// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Float3Var.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "SerializableVar.h"
#include "nsLib/Vec3.h"

class nsFloat3Var : public nsSerializableVar<nsVec3>
{
public:
    nsFloat3Var()
        : nsFloat3Var( nsVec3() ) {
    }

    nsFloat3Var( const nsVec3 &defValue )
        : nsSerializableVar( defValue ) {
    }

    bool Serialize( nsScriptSaver &ss ) override;
    bool Deserialize( script_state_t *ss ) override;
};
