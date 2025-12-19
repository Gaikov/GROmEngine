//
// Created by Roman on 10/1/2024.
//

#include "ParticlesEdgesSpawner.h"
#include "Core/ParserUtils.h"
#include "nsLib/log.h"

nsParticlesEdgesSpawner::Edge::Edge() {
    pos1.AddHandler(nsPropChangedName::CHANGED, [&](const nsBaseEvent*) {
        UpdateEdge();
    });

    pos2.AddHandler(nsPropChangedName::CHANGED, [&](const nsBaseEvent*) {
        UpdateEdge();
    });
}

void nsParticlesEdgesSpawner::Edge::UpdateEdge() {
    dir = pos2 - pos1;
    length = dir.Length();
    if (length > 0) {
        dir /= length;
    }
}

void nsParticlesEdgesSpawner::Spawn(nsParticle *p, float angle) {
    if (!_frame.empty()) {
        float dist = nsMath::Random() * _length * 0.999f;

        for (auto &e: _frame) {
            dist -= e->length;
            if (dist < 0) {
                p->pos = (e->pos1 + e->dir * (e->length + dist)).Rotate(angle);
                break;
            }
        }
    }
}

bool nsParticlesEdgesSpawner::Parse(script_state_t *ss, nsParticlesSpawnerContext *context) {

    if (ps_block_begin(ss, "edge")) {
        _frame.clear();
        _length = 0;

        do {
            Edge::sp_t e = new Edge();
            nsVec2 p1, p2;
            if (ParseFloat2(ss, "point1", p1)
                && ParseFloat2(ss, "point2", p2)) {

                e->pos1 = p1;
                e->pos2 = p2;

                _frame.push_back(e);
                _length += e->length;
            }
        } while (ps_block_next(ss));
        ps_block_end(ss);
    }

    if (_frame.empty()) {
        Log::Warning("Edges spawner is empty!");
        return false;
    }

    return true;
}
