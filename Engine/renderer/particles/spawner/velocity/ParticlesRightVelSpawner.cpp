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
    vel.Rotate(angle);
    p->vel = vel;
}

bool nsParticlesRightVelSpawner::Parse(script_state_t *ss, nsParticlesSpawnerContext *context) {
    minSpeed = ParseFloat(ss, "minSpeed");
    maxSpeed = ParseFloat(ss, "maxSpeed");
    randomDirection = ParseFloat(ss, "randomDirection") > 0;

    return true;
}
