//
// Created by Roman on 10/7/2024.
//

#include "ParticlesVelToAngleUpdater.h"

void nsParticlesVelToAngleUpdater::Update(nsParticle *p, float deltaTime) {
    const nsVec2  vel = p->vel;
    p->angle = vel.GetAngle();
}

bool nsParticlesVelToAngleUpdater::Parse(script_state_t *ss, nsParticlesUpdaterContext *context) {
    return true;
}

void nsParticlesVelToAngleUpdater::Save(nsScriptSaver *ss, nsParticlesUpdaterContext *context) {
}
