// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesMuliDirectionSpawner.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/renderer/particles/spawner/ParticlesSpawner.h"

class nsParticlesMultiDirectionSpawner : public nsParticlesSpawner {
public:
    static constexpr auto NAME = "multiDirections";
    static constexpr auto TITLE = "Multi Directions";

    nsParticlesMultiDirectionSpawner() {
        _title = TITLE;
        _name = NAME;
    }

    struct Direction {
        typedef std::shared_ptr<Direction> sp_t;

        float minAngle = 0;
        float maxAngle = 0;
        float minSpeed = 0;
        float maxSpeed = 0;
    };

    std::vector<Direction::sp_t>  directions;

    void Spawn(nsParticle *p, float angle) override;
    bool Parse(script_state_t *ss, nsParticlesSpawnerContext *context) override;
    void Save(nsScriptSaver *ss, nsParticlesSpawnerContext *context) override;
};