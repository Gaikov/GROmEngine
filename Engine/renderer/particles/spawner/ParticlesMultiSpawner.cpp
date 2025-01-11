//
// Created by Roman on 9/25/2024.
//

#include "ParticlesMultiSpawner.h"
#include "renderer/particles/spawner/factory/ParticlesSpawnerContext.h"

void nsParticlesMultiSpawner::Add(const nsParticlesSpawner::sp_t &spawner) {
    _list.push_back(spawner);
}

void nsParticlesMultiSpawner::Spawn(nsParticle *p, float angle) {
    for (auto &s: _list) {
        s->Spawn(p, angle);
    }
}

bool nsParticlesMultiSpawner::Parse(script_state_t *ss, nsParticlesSpawnerContext *context) {
    if (ps_block_begin(ss, nullptr)) {
        do {
            auto s = context->Parse(ss);
            if (s) {
                Add(s);
            }
        } while (ps_block_next(ss));
        ps_block_end(ss);
    }
    return true;
}
