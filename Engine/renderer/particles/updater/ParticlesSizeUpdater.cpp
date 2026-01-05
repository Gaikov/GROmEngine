//
// Created by Roman on 9/25/2024.
//

#include "ParticlesSizeUpdater.h"
#include "Core/ParserUtils.h"

void nsParticlesSizeUpdater::Update(nsParticle *p, float deltaTime) {
    p->size = nsMath::Lerp(p->startSize, p->startSize * scale, p->timeLine);
}

nsParticlesSizeUpdater *nsParticlesSizeUpdater::Init(float scale) {
    auto u = new nsParticlesSizeUpdater();
    u->scale = scale;
    return u;
}

bool nsParticlesSizeUpdater::Parse(script_state_t *ss, nsParticlesUpdaterContext *context) {
    scale = ParseFloat(ss, "scale", 1);
    return true;
}

void nsParticlesSizeUpdater::Save(nsScriptSaver *ss, nsParticlesUpdaterContext *context) {
    ss->VarFloat("scale", scale, 2);
}
