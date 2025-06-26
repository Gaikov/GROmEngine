// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file DebugDrawManager.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "DebugDrawPolicy.h"
#include "nsLib/SubSystem.h"

class nsDebugDrawManager : public nsSubSystem<nsDebugDrawManager> {
public:
    void AddPolicy(nsDebugDrawPolicy::sp_t policy);
    void Draw();

private:
    std::vector<nsDebugDrawPolicy::sp_t> _list;
};
