//
// Created by Roman on 2/11/2026.
//

#include "ParticlesRotationSpeedSpawner.h"

#include "Core/ParserUtils.h"

void nsParticlesRotationSpeedSpawner::Spawn(nsParticle *p, float angle) {
    p->rotationSpeed = nsMath::RandRange(minSpeed, maxSpeed);
    if (randomDirection) {
        p->rotationSpeed *= nsMath::Random() > 0.5 ? 1 : -1;
    }
}

bool nsParticlesRotationSpeedSpawner::Parse(script_state_t *ss, nsParticlesSpawnerContext *context) {
    minSpeed = ParseFloat(ss, "min_speed", 0);
    maxSpeed = ParseFloat(ss, "max_speed", M_PI);
    randomDirection = ParseBool(ss, "random_direction", false);

    return true;
}

void nsParticlesRotationSpeedSpawner::Save(nsScriptSaver *ss, nsParticlesSpawnerContext *context) {
    ss->VarFloat("min_speed", minSpeed, 0);
    ss->VarFloat("max_speed", maxSpeed, M_PI);
    ss->VarBool("random_direction", randomDirection, false);
}
