//
// Created by Roman on 10/15/2024.
//

#include "VisualParticlesBuilder.h"
#include "VisualParticles.h"
#include "renderer/particles/ParticlesManager.h"
#include "Core/ParserUtils.h"

nsVisualObject2d *nsVisualParticlesBuilder::Create(script_state_t *ss, nsVisualCreationContext2d *context) {
    return new nsVisualParticles();
}

bool nsVisualParticlesBuilder::Parse(script_state_t *ss, nsVisualObject2d *o, nsVisualCreationContext2d *context) {
    if (!nsVisualBuilder2d::Parse(ss, o, context)) {
        return false;
    }

    auto p = dynamic_cast<nsVisualParticles*>(o);
    if (!p) {
        Log::Warning("Invalid nsVisualParticles object!");
        return false;
    }

    auto &system = p->GetSystem();
    auto b = nsParticlesManager::Shared()->LoadParticles(ParseString(ss, "source"));
    if (b) {
        system.behaviour = b;
    }

    system.spawnEnabled = ParseFloat(ss, "spawn", system.spawnEnabled) != 0.0f;

    return true;
}
