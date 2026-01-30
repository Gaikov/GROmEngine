// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesMultiSpawner.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "ParticlesSpawner.h"

class nsParticlesMultiSpawner : public nsParticlesSpawner {
public:
    static constexpr auto NAME = "spawners";
    static constexpr auto TITLE = "Multi Spawners";

    nsParticlesMultiSpawner() {
        _title = TITLE;
        _name = NAME;
    }

    std::vector<sp_t>   list;

    template <class TSpawner>
    TSpawner* FindSpawner() {
        for (auto &s : list) {
            nsParticlesSpawner *spawner = s;
            if (auto res = dynamic_cast<TSpawner *>(spawner)) {
                return res;
            }
            if (const auto ms = dynamic_cast<nsParticlesMultiSpawner *>(spawner)) {
                return ms->FindSpawner<TSpawner>();
            }
        }
        return nullptr;
    }

    void Spawn(nsParticle *p, float angle) override;
    bool Parse(script_state_t *ss, nsParticlesSpawnerContext *context) override;
    void Save(nsScriptSaver *ss, nsParticlesSpawnerContext *context) override;
};