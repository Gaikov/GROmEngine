// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file DebugDrawPolicy.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include <Core/SmartPtr.h>

class nsDebugDrawPolicy {
public:
    typedef nsSmartPtr<nsDebugDrawPolicy> sp_t;

    virtual ~nsDebugDrawPolicy() = default;
    virtual void Draw() = 0;
};
