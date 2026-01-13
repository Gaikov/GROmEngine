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

void nsParticlesSizeTimelineUpdater::Validate() {
    if (timeline.size() < 2) {
        timeline.push_back(std::make_shared<Frame>());
        timeline.push_back(std::make_shared<Frame>());

        timeline[0]->time = 0;
        timeline[0]->data = 1;

        timeline[1]->time = 1;
        timeline[1]->data = 0;
    }

    if (timeline[0]->time > 0) {
        const auto frame = std::make_shared<Frame>();
        frame->time = 0;
        frame->data = 0;
        timeline.insert(timeline.begin(), frame);
    }
}
