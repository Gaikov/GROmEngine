// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesSizeUpdater.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "ParticlesUpdater.h"

class nsParticlesSizeUpdater : public nsParticlesUpdater {
public:
    static constexpr auto NAME = "sizeScale";

    float scale = 2;

    void Update(nsParticle *p, float deltaTime) override;
    bool Parse(script_state_t *ss, nsParticlesUpdaterContext *context) override;

    static nsParticlesSizeUpdater* Init(float scale);
};