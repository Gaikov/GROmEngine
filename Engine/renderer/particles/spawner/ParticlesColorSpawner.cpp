//
// Created by Roman on 12/3/2024.
//

#include "ParticlesColorSpawner.h"
#include "Core/ParserUtils.h"

void nsParticlesColorSpawner::Spawn(nsParticle *p, float angle) {
    p->color.FromLerp(color1, color2, nsMath::Random());
}

bool nsParticlesColorSpawner::Parse(script_state_t *ss, nsParticlesSpawnerContext *context) {

    ParseColor(ss, "color1", color1);
    ParseColor(ss, "color2", color2);

    return true;
}

void nsParticlesColorSpawner::Save(nsScriptSaver *ss, nsParticlesSpawnerContext *context) {
    ss->VarFloat4("color1", color1, nsColor());
    ss->VarFloat4("color2", color2, nsColor());
}
