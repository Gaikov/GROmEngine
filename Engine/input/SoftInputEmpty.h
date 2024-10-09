// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file SoftInputEmpty.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "ISoftInput.h"

class SoftInputEmpty : public ISoftInput {
    void Show() override;
    void Hide() override;

public:
    bool IsActive() override;
};