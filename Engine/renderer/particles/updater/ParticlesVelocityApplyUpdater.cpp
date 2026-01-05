//
// Created by Roman on 10/3/2024.
//

#include "ParticlesVelocityApplyUpdater.h"

void nsParticlesVelocityApplyUpdater::Update(nsParticle *p, float deltaTime) {
    p->pos += p->vel * deltaTime;
}

bool nsParticlesVelocityApplyUpdater::Parse(script_state_t *ss, nsParticlesUpdaterContext *context) {
    return true;
}

void nsParticlesVelocityApplyUpdater::Save(nsScriptSaver *ss, nsParticlesUpdaterContext *context) {
}
