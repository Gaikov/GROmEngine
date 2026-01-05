//
// Created by Roman on 9/25/2024.
//

#include "ParticlesCircleSpawner.h"
#include "Core/ParserUtils.h"
#include "renderer/particles/spawner/factory/ParticlesSpawnerContext.h"

void nsParticlesCircleSpawner::Spawn(nsParticle *p, float angle) {
    if (radius > 0.0f) {
        nsVec2  pos;
        if (onEdge) {
            pos.Random(nsMath::Lerp(minRadius, radius, nsMath::Random()));
        } else {
            pos.Random(radius * nsMath::Random());
        }
        p->pos = pos;
    } else {
        p->pos = {0, 0};
    }
}

nsParticlesSpawner *nsParticlesCircleSpawner::Init(float radius, bool onEdge) {
    auto s = new nsParticlesCircleSpawner();
    s->radius = radius;
    s->onEdge = onEdge;
    return s;
}

bool nsParticlesCircleSpawner::Parse(script_state_t *ss, nsParticlesSpawnerContext *context) {
    radius = ParseFloat(ss, "radius");
    onEdge = ParseFloat(ss, "onEdge") > 0;
    if (onEdge) {
        minRadius = ParseFloat(ss, "minRadius", radius);
    }
    return true;
}

void nsParticlesCircleSpawner::Save(nsScriptSaver *ss, nsParticlesSpawnerContext *context) {
    ss->VarFloat("radius", radius, 0);
    ss->VarBool("onEdge", onEdge, false);
    if (onEdge) {
        ss->VarFloat("minRadius", minRadius, 0);
    }
}
