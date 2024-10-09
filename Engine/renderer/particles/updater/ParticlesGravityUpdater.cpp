//
// Created by Roman on 9/26/2024.
//

#include "ParticlesGravityUpdater.h"
#include "Core/ParserUtils.h"

void nsParticlesGravityUpdater::Update(nsParticle *p, float deltaTime) {
    p->vel += gravity * deltaTime;
}

nsParticlesGravityUpdater *nsParticlesGravityUpdater::Init(const nsVec2 &gravity) {
    auto u = new nsParticlesGravityUpdater();
    u->gravity = gravity;
    return u;
}

bool nsParticlesGravityUpdater::Parse(script_state_t *ss, nsParticlesUpdaterContext *context) {
    ParseFloat2(ss, "velocity", gravity);
    return true;
}
