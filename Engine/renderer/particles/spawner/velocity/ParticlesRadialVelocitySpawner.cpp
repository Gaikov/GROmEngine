//
// Created by Roman on 9/26/2024.
//

#include "ParticlesRadialVelocitySpawner.h"
#include "Core/ParserUtils.h"

void nsParticlesRadialVelSpawner::Spawn(nsParticle *p, float angle) {
    p->vel = p->pos;
    p->vel.Norm();

    float speed = nsMath::RandRange(minSpeed, maxSpeed);
    if (randomDirection) {
        speed *= (nsMath::Random() > 0.5f ? 1.0f : -1.0f);
    }

    p->vel *= speed;
}

nsParticlesRadialVelSpawner* nsParticlesRadialVelSpawner::Init(float minSpeed, float maxSpeed, bool randomDirection) {
    auto s = new nsParticlesRadialVelSpawner();
    s->minSpeed = minSpeed;
    s->maxSpeed = maxSpeed;
    s->randomDirection = randomDirection;
    return s;
}

bool nsParticlesRadialVelSpawner::Parse(script_state_t *ss, nsParticlesSpawnerContext *context) {
    minSpeed = ParseFloat(ss, "minSpeed");
    maxSpeed = ParseFloat(ss, "maxSpeed");
    randomDirection = ParseFloat(ss, "randomDirection") > 0;
    return true;
}

void nsParticlesRadialVelSpawner::Save(nsScriptSaver *ss, nsParticlesSpawnerContext *context) {
    ss->VarFloat("minSpeed", minSpeed, 0);
    ss->VarFloat("maxSpeed", maxSpeed, 100);
    ss->VarBool("randomDirection", randomDirection, false);
}
