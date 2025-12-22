// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesEdgesSpawner.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/renderer/particles/spawner/ParticlesSpawner.h"
#include "nsLib/models/Property.h"

class nsParticlesEdgesSpawner : public nsParticlesSpawner {
public:
    static constexpr auto NAME = "edges";
    static constexpr auto TITLE = "Edges Position";

    class Edge {
        friend class nsParticlesEdgesSpawner;
    public:
        typedef nsSmartPtr<Edge> sp_t;
        typedef std::function<void()> ChangeCallback;

        Edge();

        nsProperty<nsVec2>  pos1 = nsVec2();
        nsProperty<nsVec2>  pos2 = nsVec2();

        float Length() const { return length; }

    private:
        void UpdateEdge();

        nsVec2  dir;
        float   length = 0;
    };

    nsParticlesEdgesSpawner() {
        _name = TITLE;
    }

    void Spawn(nsParticle *p, float angle) override;
    bool Parse(script_state_t *ss, nsParticlesSpawnerContext *context) override;

protected:

    std::vector<Edge::sp_t> _frame;
    float _length = 0;
};
