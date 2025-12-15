// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesMuliDirectionSpawner.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "renderer/particles/spawner/ParticlesSpawner.h"

class nsParticlesMultiDirectionSpawner : public nsParticlesSpawner {
public:
    static constexpr auto NAME = "multiDirections";

    nsParticlesMultiDirectionSpawner() {
        _name = "Multi Directions";
    }

    void Spawn(nsParticle *p, float angle) override;
    bool Parse(script_state_t *ss, nsParticlesSpawnerContext *context) override;

private:
    struct Direction {
        float minAngle = 0;
        float maxAngle = 0;
        float minSpeed = 0;
        float maxSpeed = 0;
    };

    std::vector<Direction>  _directions;
};