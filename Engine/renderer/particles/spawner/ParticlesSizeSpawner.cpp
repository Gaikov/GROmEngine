//
// Created by Roman on 9/25/2024.
//

#include "ParticlesSizeSpawner.h"
#include "Core/ParserUtils.h"
#include "renderer/particles/spawner/factory/ParticlesSpawnerContext.h"

void nsParticlesSizeSpawner::Spawn(nsParticle *p, float angle) {
    p->startSize = p->size = nsMath::RandRange(minSize, maxSize);
}

nsParticlesSizeSpawner *nsParticlesSizeSpawner::Init(float minSize, float maxSize) {
    auto s = new nsParticlesSizeSpawner();
    s->minSize = minSize;
    s->maxSize = maxSize;
    return s;
}

bool nsParticlesSizeSpawner::Parse(script_state_t *ss, nsParticlesSpawnerContext *context) {
    minSize = ParseFloat(ss, "minSize");
    maxSize = ParseFloat(ss, "maxSize");
    return true;
}
