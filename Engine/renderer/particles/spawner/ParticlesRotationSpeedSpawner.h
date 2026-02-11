// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesRotationSpeedSpawner.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "ParticlesSpawner.h"

class nsParticlesRotationSpeedSpawner : public nsParticlesSpawner {
public:
    static constexpr auto NAME = "rotation_speed";
    static constexpr auto TITLE = "Rotation Speed";

    float minSpeed = 0;
    float maxSpeed = M_PI;
    bool  randomDirection = false;

    nsParticlesRotationSpeedSpawner() {
        _title = TITLE;
        _name = NAME;
    }

    void Spawn(nsParticle *p, float angle) override;
    bool Parse(script_state_t *ss, nsParticlesSpawnerContext *context) override;
    void Save(nsScriptSaver *ss, nsParticlesSpawnerContext *context) override;
};
