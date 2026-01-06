//
// Created by Roman on 10/6/2024.
//

#include "ParticlesSizeTimelineUpdater.h"

bool nsParticlesSizeTimelineUpdater::ParseFrame(script_state_t *ss, float &frame) {
    frame = ParseFloat(ss, "scale");
    return true;
}

void nsParticlesSizeTimelineUpdater::LerpFrame(nsParticle *p, const float &f1, const float &f2, float t) {
    p->size = p->startSize * nsMath::Lerp(f1, f2, t);
}

void nsParticlesSizeTimelineUpdater::SaveFrame(nsScriptSaver *ss, float &frame) {
    ss->VarFloat("scale", frame, 0);
}
