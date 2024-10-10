// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file SoftInputKeyboard.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/input/ISoftInput.h"

class nsSoftInputKeyboard : public ISoftInput {
public:
    void Show() override;
    void Hide() override;
    bool IsActive() override;
};