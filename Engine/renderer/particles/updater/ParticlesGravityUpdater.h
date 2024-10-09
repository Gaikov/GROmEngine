// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesGravityUpdater.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "ParticlesUpdater.h"

class nsParticlesGravityUpdater : public nsParticlesUpdater {
public:
    static constexpr auto NAME = "gravity";

    nsVec2  gravity;

    bool Parse(script_state_t *ss, nsParticlesUpdaterContext *context) override;
    void Update(nsParticle *p, float deltaTime) override;

    static nsParticlesGravityUpdater* Init(const nsVec2 &gravity);
};