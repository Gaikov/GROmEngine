//
// Created by Roman on 9/26/2024.
//

#include "ParticlesPolygonSpawner.h"
#include "Core/ParserUtils.h"
#include "nsLib/log.h"
#include "renderer/particles/spawner/factory/ParticlesSpawnerContext.h"

void nsParticlesPolygonSpawner::UpdateFrame() {
    if (_frame.size() > 1) {
        _length = 0;
        for (auto i = 0; i < _frame.size(); i++) {
            const auto &e1 = _frame[i];
            const auto &e2 = _frame[(i + 1) % _frame.size()];

            e1->pos2 = e2->pos1;
            _length += e1->Length();
        }
    }
}

void nsParticlesPolygonSpawner::AddPoint(const nsVec2 &p) {
    Edge::sp_t edge = new Edge();
    edge->pos1 = p;
    _frame.push_back(edge);
    UpdateFrame();
}

bool nsParticlesPolygonSpawner::Parse(script_state_t *ss, nsParticlesSpawnerContext *context) {
    _frame.clear();
    if (ps_var_begin(ss, "point")) {
        do {
            nsVec2 point;
            if (ps_var_2f(ss, point)) {
                Edge::sp_t edge = new Edge();
                edge->pos1 = point;
                _frame.push_back(edge);
            }
        } while (ps_var_next(ss));
    }

    if (ps_var_begin(ss, "angle")) {
        do {
            float angle[2];
            if (ps_var_2f(ss, angle)) {
                auto p = nsVec2::FromAngle(nsMath::ToRad(angle[0])) * angle[1];
                Edge::sp_t edge = new Edge();
                edge->pos1 = p;
                _frame.push_back(edge);
            }
        } while (ps_var_next(ss));
    }

    if (_frame.empty()) {
        Log::Warning("Can't parse polygon spawner!");
        return false;
    }

    UpdateFrame();
    return true;
}
