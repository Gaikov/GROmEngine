// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesAngleSpawner.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "ParticlesSpawner.h"

class nsParticlesAngleSpawner : public nsParticlesSpawner {
public:
    static constexpr const char *NAME = "angle";

    float minAngle;
    float maxAngle;

    void Spawn(nsParticle *p, float angle) override;
    bool Parse(script_state_t *ss, nsParticlesSpawnerContext *context) override;

    static nsParticlesAngleSpawner* Init(float minAngle, float maxAngle);
};