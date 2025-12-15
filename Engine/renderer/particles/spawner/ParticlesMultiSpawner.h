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

    nsParticlesMultiSpawner() {
        _name = "Multi Spawners";
    }

    void Add(const nsParticlesSpawner::sp_t &spawner);
    void Spawn(nsParticle *p, float angle) override;
    bool Parse(script_state_t *ss, nsParticlesSpawnerContext *context) override;

private:
    std::vector<nsParticlesSpawner::sp_t>   _list;
};