//
// Created by Roman on 9/25/2024.
//

#include "ParticlesMultiSpawner.h"
#include "renderer/particles/spawner/factory/ParticlesSpawnerContext.h"

void nsParticlesMultiSpawner::Spawn(nsParticle *p, float angle) {
    for (auto &s: list) {
        s->Spawn(p, angle);
    }
}

bool nsParticlesMultiSpawner::Parse(script_state_t *ss, nsParticlesSpawnerContext *context) {
    if (ps_block_begin(ss, nullptr)) {
        do {
            if (auto s = context->Parse(ss)) {
                list.emplace_back(s);
            }
        } while (ps_block_next(ss));
        ps_block_end(ss);
    }
    return true;
}

void nsParticlesMultiSpawner::Save(nsScriptSaver *ss, nsParticlesSpawnerContext *context) {
    for (const auto &s: list) {
        context->Save(ss, s);
    }
}
