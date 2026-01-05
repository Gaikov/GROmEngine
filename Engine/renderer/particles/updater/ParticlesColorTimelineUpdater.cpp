//
// Created by Roman on 10/8/2024.
//

#include "ParticlesColorTimelineUpdater.h"

bool nsParticlesColorTimelineUpdater::ParseFrame(script_state_t *ss, nsColor &frame) {
    ParseColorExt(ss, "color", frame);
    return true;
}

void nsParticlesColorTimelineUpdater::LerpFrame(nsParticle *p, const nsColor &f1, const nsColor &f2, float t) {
    p->color.FromLerp(f1, f2, t);
}

void nsParticlesColorTimelineUpdater::SaveFrame(nsScriptSaver *ss, nsColor &frame) {
    ss->VarFloat4("color", frame, nsColor());
}

