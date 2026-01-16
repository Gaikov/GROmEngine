// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesColorTimelineUpdater.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "ParticlesTimelineUpdater.h"

class nsParticlesColorTimelineUpdater : public nsParticlesTimelineUpdater<nsColor> {
public:
    static constexpr auto NAME = "colorTimeline";
    static constexpr auto TITLE = "Color Timeline";

    nsParticlesColorTimelineUpdater() {
        _name = NAME;
        _title = TITLE;
    };

    void Validate() override;

protected:
    bool ParseFrame(script_state_t *ss, nsColor &frame) override;
    void LerpFrame(nsParticle *p, const nsColor &f1, const nsColor &f2, float t) override;
    void SaveFrame(nsScriptSaver *ss, nsColor &frame) override;
};