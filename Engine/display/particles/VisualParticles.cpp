//
// Created by Roman on 10/15/2024.
//

#include "VisualParticles.h"
#include "TimeFormat.h"

void nsVisualParticles::GetLocalBounds(nsRect &bounds) {
    bounds = {0, 0, 1, 1};
    //TODO: calculate bounds
}

void nsVisualParticles::Loop() {
    _system.Update(g_frameTime);
}

void nsVisualParticles::DrawContent(const nsVisualContext2d &context) {
    _system.Draw();
}
