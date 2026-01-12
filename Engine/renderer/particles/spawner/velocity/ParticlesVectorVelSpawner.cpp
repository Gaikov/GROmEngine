//
// Created by Roman on 9/26/2024.
//

#include "ParticlesVectorVelSpawner.h"
#include "Core/ParserUtils.h"

void nsParticlesVectorVelSpawner::Spawn(nsParticle *p, const float angle) {
    float speed = nsMath::RandRange(minSpeed, maxSpeed);
    if (randomDirection) {
        if (nsMath::Random() > 0.5) {
            speed = -speed;
        }
    }

    p->vel = (direction * speed).Rotate(angle);
}

nsParticlesVectorVelSpawner *
nsParticlesVectorVelSpawner::Init(const nsVec2 &direction, const float minSpeed, const float maxSpeed, const bool randomDirection) {
    const auto s = new nsParticlesVectorVelSpawner();
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

void nsParticlesVectorVelSpawner::Save(nsScriptSaver *ss, nsParticlesSpawnerContext *context) {
    ss->VarFloat2("direction", direction, nsVec2());
    ss->VarFloat("minSpeed", minSpeed, 0);
    ss->VarFloat("maxSpeed", maxSpeed, 100);
    ss->VarBool("randomDirection", randomDirection, false);
}
