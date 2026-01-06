//
// Created by Roman on 9/25/2024.
//

#include "ParticlesMultiUpdater.h"
#include "renderer/particles/updater/factory/ParticlesUpdaterContext.h"

void nsParticlesCompositeUpdater::Update(nsParticle *p, float deltaTime) {
    for (auto &u: list) {
        u->Update(p, deltaTime);
    }
}

bool nsParticlesCompositeUpdater::Parse(script_state_t *ss, nsParticlesUpdaterContext *context) {
    if (ps_block_begin(ss, nullptr)) {
        do {
            auto u = context->Parse(ss);
            if (u) {
                list.push_back(u);
            }
        } while (ps_block_next(ss));
        ps_block_end(ss);
    }
    return true;
}

void nsParticlesCompositeUpdater::Save(nsScriptSaver *ss, nsParticlesUpdaterContext *context) {
    for (auto &u: list) {
        context->Save(ss, u);
    }
}