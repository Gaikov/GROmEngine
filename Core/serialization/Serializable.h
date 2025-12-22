// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Serializable.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Core/ScriptSaver.h"
#include "Core/Parser.h"
#include "nsLib/utils/NoCopyable.h"

class nsSerializable : public nsNoCopyable {
public:
    virtual void ResetDefault() = 0;
    virtual bool Serialize(nsScriptSaver &ss) = 0;
    virtual bool Deserialize(script_state_t *ss) = 0;
};
