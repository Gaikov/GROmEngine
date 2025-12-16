// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesColorSpawner.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "ParticlesSpawner.h"

class nsParticlesColorSpawner : public nsParticlesSpawner {
public:
    static constexpr auto NAME = "color";
    static constexpr auto TITLE = "Random Color";

    nsColor color1, color2;

    nsParticlesColorSpawner() {
        _name = TITLE;
    }

    void Spawn(nsParticle *p, float angle) override;
    bool Parse(script_state_t *ss, nsParticlesSpawnerContext *context) override;
};