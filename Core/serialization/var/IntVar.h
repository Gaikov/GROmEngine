// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
#pragma once

#include "SerializableVar.h"

class nsIntVar final : public nsSerializableVar<int> {
public:
    nsIntVar() : nsIntVar( 0 ) {}
    nsIntVar( const int defaultValue ) : nsSerializableVar( defaultValue ) {}

    nsIntVar &operator=( const int value ) {
        SetValue( value );
        return *this;
    }

    bool Serialize( nsScriptSaver &saver ) override;
    bool Deserialize( script_state_t *state ) override;
};
