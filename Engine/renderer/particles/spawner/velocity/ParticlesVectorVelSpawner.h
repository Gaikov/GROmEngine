// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesVectorVelSpawner.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/renderer/particles/spawner/ParticlesSpawner.h"

class nsParticlesVectorVelSpawner : public nsParticlesSpawner {
public:
    static constexpr const char *NAME = "vectorVel";

    nsVec2  direction = {1, 0};
    float   minSpeed;
    float   maxSpeed;
    bool    randomDirection;

    void Spawn(nsParticle *p, float angle) override;
    bool Parse(script_state_t *ss, nsParticlesSpawnerContext *context) override;

    static nsParticlesVectorVelSpawner* Init(const nsVec2 &direction, float minSpeed, float maxSpeed, bool randomDirection);
};