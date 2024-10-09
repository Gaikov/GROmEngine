//
// Created by Roman on 10/1/2024.
//

#include "ParticlesEdgesSpawner.h"
#include "Core/ParserUtils.h"
#include "nsLib/log.h"

void nsParticlesEdgesSpawner::Spawn(nsParticle *p, float angle) {
    if (_frame.size() > 1) {
        float dist = nsMath::Random() * _length * 0.999f;

        for (auto &e: _frame) {
            dist -= e.length;
            if (dist < 0) {
                p->pos = (e.pos + e.dir * (e.length + dist)).Rotate(angle);
                return;
            }
        }
    }
}

bool nsParticlesEdgesSpawner::Parse(script_state_t *ss, nsParticlesSpawnerContext *context) {

    if (ps_block_begin(ss, "edge")) {
        _frame.clear();
        _length = 0;

        do {
            Edge e;
            nsVec2 p2;
            if (ParseFloat2(ss, "point1", e.pos)
                && ParseFloat2(ss, "point2", p2)) {

                e.dir = p2 - e.pos;
                e.length = e.dir.Length();
                e.dir /= e.length;

                _frame.push_back(e);
                _length += e.length;
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
