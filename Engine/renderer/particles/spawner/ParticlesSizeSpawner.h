// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesSizeSpawner.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "ParticlesSpawner.h"

class nsParticlesSizeSpawner : public nsParticlesSpawner {
public:
    static constexpr const char *NAME = "size";
    static constexpr auto TITLE = "Random Size";

    float minSize = 10;
    float maxSize = 100;

    nsParticlesSizeSpawner() {
        _name = TITLE;
    };

    void Spawn(nsParticle *p, float angle) override;
    bool Parse(script_state_t *ss, nsParticlesSpawnerContext *context) override;

    static nsParticlesSizeSpawner* Init(float minSize, float maxSize);
};