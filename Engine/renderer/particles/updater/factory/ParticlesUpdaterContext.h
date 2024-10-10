// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesUpdaterContext.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/renderer/particles/updater/ParticlesUpdater.h"
#include "Core/Parser.h"

class nsParticlesUpdaterContext {
public:
    virtual nsParticlesUpdater* Parse(script_state_t *ss) = 0;
};