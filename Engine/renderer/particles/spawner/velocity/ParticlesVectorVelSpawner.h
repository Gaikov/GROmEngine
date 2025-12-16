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
    static constexpr auto TITLE = "Vector Velocity";

    nsVec2  direction = {1, 0};
    float   minSpeed = 0;
    float   maxSpeed = 100;
    bool    randomDirection = false;

    nsParticlesVectorVelSpawner() {
        _name = TITLE;
    }

    void Spawn(nsParticle *p, float angle) override;
    bool Parse(script_state_t *ss, nsParticlesSpawnerContext *context) override;

    static nsParticlesVectorVelSpawner* Init(const nsVec2 &direction, float minSpeed, float maxSpeed, bool randomDirection);
};