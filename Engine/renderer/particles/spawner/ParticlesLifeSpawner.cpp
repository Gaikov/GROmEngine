//
// Created by Roman on 9/25/2024.
//

#include "ParticlesLifeSpawner.h"
#include "Core/ParserUtils.h"
#include "renderer/particles/spawner/factory/ParticlesSpawnerContext.h"

void nsParticlesLifeSpawner::Spawn(nsParticle *p, float angle) {
    p->maxTime = p->timeLeft = nsMath::RandRange(minLifeTime, maxLifeTime);
}

nsParticlesLifeSpawner *nsParticlesLifeSpawner::Init(float minTime, float maxTime) {
    auto s = new nsParticlesLifeSpawner();
    s->minLifeTime = minTime;
    s->maxLifeTime = maxTime;
    return s;
}

bool nsParticlesLifeSpawner::Parse(script_state_t *ss, nsParticlesSpawnerContext *context) {
    minLifeTime = ParseFloat(ss, "minTime");
    maxLifeTime = ParseFloat(ss, "maxTime");
    return true;
}
