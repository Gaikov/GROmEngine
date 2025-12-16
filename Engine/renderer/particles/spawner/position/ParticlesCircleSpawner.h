// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesPointSpawner.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/renderer/particles/spawner/ParticlesSpawner.h"

class nsParticlesCircleSpawner : public nsParticlesSpawner {
public:
    static constexpr const char *NAME = "circle";
    static constexpr auto TITLE = "Circle Position";

    float   radius = 0;
    float   minRadius = 0;
    bool    onEdge = false;

    nsParticlesCircleSpawner() {
        _name = TITLE;
    }

    void Spawn(nsParticle *p, float angle) override;
    bool Parse(script_state_t *ss, nsParticlesSpawnerContext *context) override;

    static nsParticlesSpawner* Init(float radius, bool onEdge);
};