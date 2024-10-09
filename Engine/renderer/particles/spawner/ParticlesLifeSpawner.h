// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesLifeSpawner.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "ParticlesSpawner.h"

class nsParticlesLifeSpawner : public nsParticlesSpawner {
public:
    static constexpr const char *NAME = "life";

    float minLifeTime = 0.1;
    float maxLifeTime = 0.2;

    void Spawn(nsParticle *p, float angle) override;
    bool Parse(script_state_t *ss, nsParticlesSpawnerContext *context) override;

    static nsParticlesLifeSpawner* Init(float minTime, float maxTime);
};