//
// Created by Roman on 9/26/2024.
//

#include "ParticlesAngleSpawner.h"
#include "Core/ParserUtils.h"
#include "renderer/particles/spawner/factory/ParticlesSpawnerContext.h"

void nsParticlesAngleSpawner::Spawn(nsParticle *p, float angle) {
    p->angle = nsMath::RandRange(minAngle, maxAngle);
}

nsParticlesAngleSpawner *nsParticlesAngleSpawner::Init(float minAngle, float maxAngle) {
    const auto s = new nsParticlesAngleSpawner();
    s->minAngle = minAngle;
    s->maxAngle = maxAngle;
    return s;
}

bool nsParticlesAngleSpawner::Parse(script_state_t *ss, nsParticlesSpawnerContext *context) {
    minAngle = nsMath::ToRad(ParseFloat(ss, "minAngle", 0));
    maxAngle = nsMath::ToRad(ParseFloat(ss, "maxAngle", 0));
    return true;
}

void nsParticlesAngleSpawner::Save(nsScriptSaver *ss, nsParticlesSpawnerContext *context) {
    ss->VarFloat("minAngle", nsMath::ToDeg(minAngle), 0);
    ss->VarFloat("maxAngle", nsMath::ToDeg(maxAngle), 0);
}
