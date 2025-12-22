// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file SerializeUtils.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "Serializable.h"
#include "Core/ScriptSaver.h"

class nsSerializeUtils final {
public:
    static bool SerializeVar(nsScriptSaver &ss, const char *name, nsSerializable *var);
    static bool DeserializeVar(script_state_t *ss, const char *name, nsSerializable *var);

};
