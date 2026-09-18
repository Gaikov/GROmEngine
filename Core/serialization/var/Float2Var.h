// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Float2Var.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "SerializableVar.h"
#include "nsLib/Vec2.h"

class nsFloat2Var : public nsSerializableVar<nsVec2>
{
public:
    nsFloat2Var( const nsVec2 &defValue )
        : nsSerializableVar( defValue ) {
    }

    bool Serialize( nsScriptSaver &ss ) override;
    bool Deserialize( script_state_t *ss ) override;
};
