// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file EnumVar.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "SerializableVar.h"

template<typename TEnum>
class nsEnumVar : public nsSerializableVar<TEnum> {
public:
    nsEnumVar(const TEnum &defValue)
        : nsSerializableVar<TEnum>(defValue) {
    }

    nsEnumVar& operator = (const TEnum &value) {
        this->SetValue(value);
        return *this;
    }

    bool Serialize(nsScriptSaver &ss) override;
    bool Deserialize(script_state_t *ss) override;
};

template<typename TEnum>
bool nsEnumVar<TEnum>::Serialize(nsScriptSaver &ss) {
    TEnum value = this->GetValue();
    ss.VarValue("%d", static_cast<int>(value));
    return true;
}

template<typename TEnum>
bool nsEnumVar<TEnum>::Deserialize(script_state_t *ss) {
    auto value = static_cast<TEnum>(ps_var_f(ss));
    this->SetValue(value);
    return true;
}
