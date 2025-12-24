// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesLoader.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/renderer/particles/ParticlesBehaviour.h"

class nsParticlesLoader {
public:
    virtual ~nsParticlesLoader() = default;
    virtual nsParticlesBehaviour* LoadParticles(const char *fileName) = 0;
};
