// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesMultiSpawner.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "ParticlesSpawner.h"

class nsParticlesMultiSpawner : public nsParticlesSpawner {
public:
    static constexpr auto NAME = "spawners";
    static constexpr auto TITLE = "Multi Spawners";

    nsParticlesMultiSpawner() {
        _name = TITLE;
    }

    std::vector<nsParticlesSpawner::sp_t>   list;

    void Spawn(nsParticle *p, float angle) override;
    bool Parse(script_state_t *ss, nsParticlesSpawnerContext *context) override;
};