// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesEdgesSpawner.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "engine/renderer/particles/spawner/ParticlesSpawner.h"

class nsParticlesEdgesSpawner : public nsParticlesSpawner {
public:
    static constexpr auto NAME = "edges";

    void Spawn(nsParticle *p, float angle) override;
    bool Parse(script_state_t *ss, nsParticlesSpawnerContext *context) override;

protected:
    struct Edge {
        nsVec2  pos;
        nsVec2  dir;
        float   length = 0;
    };

    std::vector<Edge> _frame;
    float _length = 0;
};