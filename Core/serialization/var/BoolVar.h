// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file BoolVar.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "SerializableVar.h"

class nsBoolVar : public nsSerializableVar<bool> {
public:
    nsBoolVar(const bool defValue)
        : nsSerializableVar(defValue) {
    }

    nsBoolVar& operator = (const bool value) {
        SetValue(value);
        return *this;
    }

    bool Serialize(nsScriptSaver &ss) override;
    bool Deserialize(script_state_t *ss) override;
};
