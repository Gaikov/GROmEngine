//
// Created by Roman on 9/26/2024.
//

#include "ParticlesGravityUpdater.h"
#include "Core/ParserUtils.h"

void nsParticlesGravityUpdater::Update(nsParticle *p, float deltaTime) {
    p->vel += gravity * deltaTime;
}

void nsParticlesGravityUpdater::Save(nsScriptSaver *ss, nsParticlesUpdaterContext *context) {
    ss->VarFloat2("velocity", gravity, nsVec2());
}

nsParticlesGravityUpdater *nsParticlesGravityUpdater::Init(const nsVec2 &gravity) {
    const auto u = new nsParticlesGravityUpdater();
    u->gravity = gravity;
    return u;
}

bool nsParticlesGravityUpdater::Parse(script_state_t *ss, nsParticlesUpdaterContext *context) {
    ParseFloat2(ss, "velocity", gravity);
    return true;
}
