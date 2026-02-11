// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesRotationUpdater.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "ParticlesUpdater.h"

class nsParticlesRotationUpdater final : public nsParticlesUpdater {
public:
    static constexpr auto NAME = "rotation";
    static constexpr auto TITLE = "Rotation";

    nsParticlesRotationUpdater() {
        _name = NAME;
        _title = TITLE;
    }

    bool slowDown = false;

    void Update(nsParticle *p, float deltaTime) override;
    bool Parse(script_state_t *ss, nsParticlesUpdaterContext *context) override;
    void Save(nsScriptSaver *ss, nsParticlesUpdaterContext *context) override;
};
