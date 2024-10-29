// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesVelDumpUpdater.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "renderer/particles/updater/ParticlesUpdater.h"

class nsParticlesVelDampUpdater : public nsParticlesUpdater {
public:
    static constexpr auto NAME = "velDamp";

    void Update(nsParticle *p, float deltaTime) override;
    bool Parse(script_state_t *ss, nsParticlesUpdaterContext *context) override;
private:
    float _value = 1;
};