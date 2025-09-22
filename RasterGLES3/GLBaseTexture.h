// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file GLBaseTexture.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/RenDevice.h"

class nsGLBaseTexture : public ITexture {
public:
    virtual bool Bind() = 0;
};
