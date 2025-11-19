// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file StringVar.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "SerializableVar.h"

class nsStringVar : public nsSerializableVar<std::string> {
public:
    nsStringVar(const char *name, const std::string &defValue)
        : nsSerializableVar(name, defValue) {
    }

    nsStringVar& operator = (const char *value) {
        SetValue(value);
        return *this;
    }

    bool Serialize(nsScriptSaver &ss) override;
    bool Deserialize(script_state_t *ss) override;
};
