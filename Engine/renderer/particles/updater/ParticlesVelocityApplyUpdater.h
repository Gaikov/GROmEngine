// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesVelocityApplyUpdater.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "ParticlesUpdater.h"

class nsParticlesVelocityApplyUpdater : public nsParticlesUpdater {
public:
    static constexpr auto NAME = "velApply";
    static constexpr auto TITLE = "Velocity Apply";

    nsParticlesVelocityApplyUpdater() {
        _name = NAME;
        _title = TITLE;
    };

    void Update(nsParticle *p, float deltaTime) override;
    bool Parse(script_state_t *ss, nsParticlesUpdaterContext *context) override;
    void Save(nsScriptSaver *ss, nsParticlesUpdaterContext *context) override;
};