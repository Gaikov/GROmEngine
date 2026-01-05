// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesVelDumpUpdater.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/renderer/particles/updater/ParticlesUpdater.h"

class nsParticlesVelDampUpdater : public nsParticlesUpdater {
public:
    static constexpr auto NAME = "velDamp";

    float value = 1;

    nsParticlesVelDampUpdater() {
        _name = NAME;
    };

    void Update(nsParticle *p, float deltaTime) override;
    bool Parse(script_state_t *ss, nsParticlesUpdaterContext *context) override;
    void Save(nsScriptSaver *ss, nsParticlesUpdaterContext *context) override;

};