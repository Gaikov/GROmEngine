// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesEdgesSpawner.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Core/serialization/var/ArrayVar.h"
#include "Engine/renderer/particles/spawner/ParticlesSpawner.h"
#include "nsLib/models/Property.h"

class nsParticlesEdgesSpawner : public nsParticlesSpawner {
public:
    static constexpr auto NAME = "edges";
    static constexpr auto TITLE = "Edges Position";

    class nsPoint final : public nsProperty<nsVec2> {
        friend class nsParticlesEdgesSpawner;
    public:
        typedef std::shared_ptr<nsPoint> sp_t;
        typedef std::function<void()> callback_t;


        nsPoint() : nsPoint(nsVec2()) {};
        nsPoint(const nsVec2 &value) : nsProperty(value) {
            AddHandler(nsPropChangedName::CHANGED, [this](const nsBaseEvent *e) {
                if (owner) {
                    owner->_valid = false;
                }
            });
        }

    private:
        nsParticlesEdgesSpawner *owner = nullptr;
    };

    nsVector<nsPoint::sp_t> points;

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
};
