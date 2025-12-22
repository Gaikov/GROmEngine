// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file SerializableFile.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "SerializableGroup.h"

class nsSerializableFile : public nsSerializableGroup {
public:
    virtual bool Load(const char *fileName);
    virtual bool Save(const char *fileName);
};
