// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesRadialVelocitySpawner.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "engine/renderer/particles/spawner/ParticlesSpawner.h"

class nsParticlesRadialVelSpawner : public nsParticlesSpawner {
public:
    static constexpr const char *NAME = "radialVel";

    float minSpeed;
    float maxSpeed;
    bool  randomDirection;

    void Spawn(nsParticle *p, float angle) override;
    bool Parse(script_state_t *ss, nsParticlesSpawnerContext *context) override;

    static nsParticlesRadialVelSpawner* Init(float minSpeed, float maxSpeed, bool randomDirection);
};