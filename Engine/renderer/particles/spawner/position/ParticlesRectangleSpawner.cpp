//
// Created by Roman on 2/12/2026.
//

#include "ParticlesRectangleSpawner.h"
#include "Core/ParserUtils.h"

void nsParticlesRectangleSpawner::Spawn(nsParticle *p, float angle) {
    const auto hw = width / 2;
    const auto hh = height / 2;

    if (onEdge) {
        p->pos.x = nsMath::RandRange(-hw, hw);
        p->pos.y = nsMath::RandRange(-hh, hh);
    } else if (nsMath::Random() > 0.5f) {
        p->pos.x = nsMath::RandRange(-hw, hw);
        p->pos.y = hh * nsMath::RandomSign();
    } else {
        p->pos.x = hw * nsMath::RandomSign();
        p->pos.y = nsMath::RandRange(-hh, hh);
    }
}

bool nsParticlesRectangleSpawner::Parse(script_state_t *ss, nsParticlesSpawnerContext *context) {
    width = ParseFloat(ss, "width", 100);
    height = ParseFloat(ss, "height", 100);
    onEdge = ParseBool(ss, "on_edge", false);
    return true;
}

void nsParticlesRectangleSpawner::Save(nsScriptSaver *ss, nsParticlesSpawnerContext *context) {
    ss->VarFloat("width", width, 100);
    ss->VarFloat("height", height, 100);
    ss->VarBool("on_edge", onEdge, false);
}
