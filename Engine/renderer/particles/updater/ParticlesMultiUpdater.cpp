//
// Created by Roman on 9/25/2024.
//

#include "ParticlesMultiUpdater.h"
#include "renderer/particles/updater/factory/ParticlesUpdaterContext.h"

void nsParticlesCompositeUpdater::Add(nsParticlesUpdater::sp_t u) {
    _list.push_back(u);
}

void nsParticlesCompositeUpdater::Update(nsParticle *p, float deltaTime) {
    for (auto &u: _list) {
        u->Update(p, deltaTime);
    }
}

bool nsParticlesCompositeUpdater::Parse(script_state_t *ss, nsParticlesUpdaterContext *context) {
    if (ps_block_begin(ss, nullptr)) {
        do {
            auto u = context->Parse(ss);
            if (u) {
                Add(u);
            }
        } while (ps_block_next(ss));
        ps_block_end(ss);
    }
    return true;
}

void nsParticlesCompositeUpdater::Save(nsScriptSaver *ss, nsParticlesUpdaterContext *context) {
    for (auto &u: _list) {
        if (ss->BlockBegin(GetName())) {
            context->Save(ss, u);
            ss->BlockEnd();
        }
    }
}