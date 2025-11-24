// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ColorVar.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "SerializableVar.h"
#include "nsLib/color.h"

class nsColorVar : public  nsSerializableVar<nsColor> {

public:
    nsColorVar(const char *name, const nsColor &defValue)
        : nsSerializableVar(name, defValue) {
    }
    bool Serialize(nsScriptSaver &ss) override;
    bool Deserialize(script_state_t *ss) override;
};
