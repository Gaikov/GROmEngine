// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesGravityUpdater.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "ParticlesUpdater.h"

class nsParticlesGravityUpdater : public nsParticlesUpdater {
public:
    static constexpr auto NAME = "gravity";
    static constexpr auto TITLE = "Gravity";

    nsVec2  gravity;

    nsParticlesGravityUpdater() {
        _name = NAME;
        _title = TITLE;
    };

    bool Parse(script_state_t *ss, nsParticlesUpdaterContext *context) override;
    void Update(nsParticle *p, float deltaTime) override;
    void Save(nsScriptSaver *ss, nsParticlesUpdaterContext *context) override;

    static nsParticlesGravityUpdater* Init(const nsVec2 &gravity);
};