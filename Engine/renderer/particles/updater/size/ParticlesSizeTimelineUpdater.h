// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesSizeTimelineUpdater.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "renderer/particles/updater/ParticlesTimelineUpdater.h"

class nsParticlesSizeTimelineUpdater : public nsParticlesTimelineUpdater<float> {
public:
    static constexpr auto NAME = "sizeTimeline";

protected:
    bool ParseFrame(script_state_t *ss, float &frame) override;
    void LerpFrame(nsParticle *p, const float &f1, const float &f2, float t) override;
};