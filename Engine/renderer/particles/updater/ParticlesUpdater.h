// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesUpdater.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Core/SmartPtr.h"
#include "Core/Parser.h"
#include "Engine/renderer/particles/Particle.h"

class nsParticlesUpdaterContext;

class nsParticlesUpdater {
public:
    virtual ~nsParticlesUpdater() = default;

    typedef nsSmartPtr<nsParticlesUpdater>  sp_t;

    virtual void Update(nsParticle *p, float deltaTime) = 0;
    virtual bool Parse(script_state_t *ss, nsParticlesUpdaterContext *context) = 0;
};