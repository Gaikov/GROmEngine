// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesMultiUpdater.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "ParticlesUpdater.h"

class nsParticlesCompositeUpdater : public nsParticlesUpdater {
public:
    static constexpr auto NAME = "updaters";
    static constexpr auto TITLE = "Multi Updaters";

    nsParticlesCompositeUpdater() {
        _name = NAME;
        _title = TITLE;
    };

    std::vector<sp_t>   list;

    void Update(nsParticle *p, float deltaTime) override;
    bool Parse(script_state_t *ss, nsParticlesUpdaterContext *context) override;
    void Save(nsScriptSaver *ss, nsParticlesUpdaterContext *context) override;
};