//
// Created by Roman on 9/26/2024.
//

#include "ParticlesVectorVelSpawner.h"
#include "Core/ParserUtils.h"

void nsParticlesVectorVelSpawner::Spawn(nsParticle *p, float angle) {
    float speed = nsMath::RandRange(minSpeed, maxSpeed);
    if (randomDirection) {
        if (nsMath::Random() > 0.5) {
            speed = -speed;
        }
    }

    p->vel = (direction * speed).Rotate(angle);
}

nsParticlesVectorVelSpawner *
nsParticlesVectorVelSpawner::Init(const nsVec2 &direction, float minSpeed, float maxSpeed, bool randomDirection) {
    auto s = new nsParticlesVectorVelSpawner();
    s->direction = direction;
    s->direction.Norm();

    s->minSpeed = minSpeed;
    s->maxSpeed = maxSpeed;
    s->randomDirection = randomDirection;

    return s;
}

bool nsParticlesVectorVelSpawner::Parse(script_state_t *ss, nsParticlesSpawnerContext *context) {
    ParseFloat2(ss, "direction", direction);
    minSpeed = ParseFloat(ss, "minSpeed");
    maxSpeed = ParseFloat(ss, "maxSpeed");
    randomDirection = ParseFloat(ss, "randomDirection") > 0;
    return true;
}
