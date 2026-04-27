// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesRectangleSpawner.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/renderer/particles/spawner/ParticlesSpawner.h"

class nsParticlesRectangleSpawner final : public nsParticlesSpawner {
public:
    static constexpr const char *NAME = "rectangle";
    static constexpr auto TITLE = "Rectangle Position";

    float width = 100;
    float height = 100;
    bool onEdge = false;

    nsParticlesRectangleSpawner() {
        _title = TITLE;
        _name = NAME;
    }

    void Spawn(nsParticle *p, float angle) override;
    bool Parse(script_state_t *ss, nsParticlesSpawnerContext *context) override;
    void Save(nsScriptSaver *ss, nsParticlesSpawnerContext *context) override;
};
