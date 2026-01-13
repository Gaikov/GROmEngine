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

void nsParticlesColorTimelineUpdater::Validate() {
    if (timeline.size() < 2) {
        timeline.push_back(std::make_shared<Frame>());
        timeline.push_back(std::make_shared<Frame>());

        timeline[0]->time = 0;
        timeline[0]->data = nsColor::magenta;

        timeline[1]->time = 1;
        timeline[1]->data = nsColor::blue;
    }

    if (timeline[0]->time > 0) {
        const auto frame = std::make_shared<Frame>();
        frame->time = 0;
        frame->data = nsColor();
        timeline.insert(timeline.begin(), frame);
    }
}
