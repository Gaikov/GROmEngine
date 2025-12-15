// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesRightVelSpawner.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "renderer/particles/spawner/ParticlesSpawner.h"

class nsParticlesRightVelSpawner : public nsParticlesSpawner {
public:
    static constexpr auto NAME = "rightVel";

    float minSpeed = 0;
    float maxSpeed = 0;
    bool randomDirection = false;

    nsParticlesRightVelSpawner() {
        _name = "Twisting Velocity";
    }

    void Spawn(nsParticle *p, float angle) override;
    bool Parse(script_state_t *ss, nsParticlesSpawnerContext *context) override;
};