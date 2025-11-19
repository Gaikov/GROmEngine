// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file SerializableFile.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "SerializableGroup.h"

class nsSerializableFile : public nsSerializableGroup {
public:
    explicit nsSerializableFile(const char *name);

    bool Load(const char *fileName);
    bool Save(const char *fileName);
};
