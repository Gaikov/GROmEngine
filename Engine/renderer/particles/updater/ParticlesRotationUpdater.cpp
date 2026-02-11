//
// Created by Roman on 2/11/2026.
//

#include "ParticlesRotationUpdater.h"
#include "Core/ParserUtils.h"

void nsParticlesRotationUpdater::Update(nsParticle *p, const float deltaTime) {
    float speed = p->rotationSpeed;
    if (slowDown) {
        speed *= (1 - p->timeLine);
    }
    p->angle += speed * deltaTime;
}

bool nsParticlesRotationUpdater::Parse(script_state_t *ss, nsParticlesUpdaterContext *context) {
    slowDown = ParseBool(ss, "slow_down", false);
    return true;
}

void nsParticlesRotationUpdater::Save(nsScriptSaver *ss, nsParticlesUpdaterContext *context) {
    ss->VarBool("slow_down", slowDown, false);
}
