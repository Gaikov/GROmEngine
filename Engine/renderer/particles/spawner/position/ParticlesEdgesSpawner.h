// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesEdgesSpawner.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Core/serialization/var/ArrayVar.h"
#include "Engine/renderer/particles/spawner/ParticlesSpawner.h"
#include "nsLib/models/Point.h"
#include "nsLib/models/Property.h"

class nsParticlesEdgesSpawner : public nsParticlesSpawner, public nsPoint::Owner {
public:
    static constexpr auto NAME = "edges";
    static constexpr auto TITLE = "Edges Position";

    nsArray<nsPoint::sp_t> points;

    nsParticlesEdgesSpawner();

    void Spawn(nsParticle *p, float angle) override;
    bool Parse(script_state_t *ss, nsParticlesSpawnerContext *context) override;
    void Save(nsScriptSaver *ss, nsParticlesSpawnerContext *context) override;

protected:

    struct Edge {
        nsVec2  pos;
        nsVec2  dir;
        float   length = 0;
    };

    bool _valid = false;
    std::vector<Edge> _frame;
    float _length = 0;

    virtual void Validate();
    void OnPointChanged(const nsPoint &point) override;
};
