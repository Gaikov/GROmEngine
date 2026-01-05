// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesRightVelSpawner.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/renderer/particles/spawner/ParticlesSpawner.h"

class nsParticlesRightVelSpawner : public nsParticlesSpawner {
public:
    static constexpr auto NAME = "rightVel";
    static constexpr auto TITLE = "Twisting Velocity";

    float minSpeed = 0;
    float maxSpeed = 0;
    bool randomDirection = false;

    nsParticlesRightVelSpawner() {
        _title = TITLE;
        _name = NAME;
    }

    void Spawn(nsParticle *p, float angle) override;
    bool Parse(script_state_t *ss, nsParticlesSpawnerContext *context) override;
    void Save(nsScriptSaver *ss, nsParticlesSpawnerContext *context) override;
};