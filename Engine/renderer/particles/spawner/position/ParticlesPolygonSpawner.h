// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesPolygonSpawner.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "ParticlesEdgesSpawner.h"

class nsParticlesPolygonSpawner : public nsParticlesEdgesSpawner {
public:
    static constexpr const char *NAME = "polygon";
    static constexpr auto TITLE = "Polygon Position";

    nsParticlesPolygonSpawner() {
        _title = TITLE;
        _name = NAME;
    }

    void AddPoint(const nsVec2 &p);
    bool Parse(script_state_t *ss, nsParticlesSpawnerContext *context) override;
    void Save(nsScriptSaver *ss, nsParticlesSpawnerContext *context) override;

protected:
    void UpdateFrame();
};