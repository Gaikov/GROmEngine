// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file FloatVar.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "SerializableVar.h"

class nsFloatVar : public nsSerializableVar<float> {
public:
    nsFloatVar(const float defValue)
        : nsSerializableVar(defValue) {
    }

    nsFloatVar& operator = (const float value) {
        SetValue(value);
        return *this;
    }

    bool Serialize(nsScriptSaver &ss) override;
    bool Deserialize(script_state_t *ss) override;
};
