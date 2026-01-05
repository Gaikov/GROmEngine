//
// Created by Roman on 10/1/2024.
//

#include "ParticlesRightVelSpawner.h"
#include "Core/ParserUtils.h"

void nsParticlesRightVelSpawner::Spawn(nsParticle *p, float angle) {
    nsVec2 vel = nsVec2(p->pos).GetRight();
    vel.Norm();
    float speed = nsMath::RandRange(minSpeed, maxSpeed);
    if (randomDirection) {
        if (nsMath::Random() > 0.5) {
            speed = -speed;
        }
    }
    vel *= speed;
    p->vel = vel.Rotate(angle);
}

bool nsParticlesRightVelSpawner::Parse(script_state_t *ss, nsParticlesSpawnerContext *context) {
    minSpeed = ParseFloat(ss, "minSpeed");
    maxSpeed = ParseFloat(ss, "maxSpeed");
    randomDirection = ParseFloat(ss, "randomDirection") > 0;

    return true;
}

void nsParticlesRightVelSpawner::Save(nsScriptSaver *ss, nsParticlesSpawnerContext *context) {
    ss->VarFloat("minSpeed", minSpeed, 0);
    ss->VarFloat("maxSpeed", maxSpeed, 0);
    ss->VarBool("randomDirection", randomDirection, false);
}
