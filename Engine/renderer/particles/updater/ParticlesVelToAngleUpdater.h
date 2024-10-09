// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesVelToAngleUpdater.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "engine/renderer/particles/updater/factory/ParticlesUpdaterContext.h"

class nsParticlesVelToAngleUpdater : public nsParticlesUpdater {
public:
    static constexpr auto NAME = "velAngle";

    void Update(nsParticle *p, float deltaTime) override;
    bool Parse(script_state_t *ss, nsParticlesUpdaterContext *context) override;
};