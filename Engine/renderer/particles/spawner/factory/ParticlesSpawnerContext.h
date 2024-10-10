// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesSpawnerContext.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/renderer/particles/spawner/ParticlesSpawner.h"

class nsParticlesSpawnerContext {
public:
    virtual nsParticlesSpawner* Parse(script_state_t *ss) = 0;
};