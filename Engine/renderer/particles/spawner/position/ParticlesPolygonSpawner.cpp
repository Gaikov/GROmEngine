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
            auto &e1 = _frame[i];
            auto &e2 = _frame[(i + 1) % _frame.size()];

            e1.dir = e2.pos - e1.pos;
            e1.length = e1.dir.Length();
            e1.dir /= e1.length;

            _length += e1.length;
        }
    }
}

void nsParticlesPolygonSpawner::AddPoint(const nsVec2 &p) {
    Edge e = {p};
    _frame.push_back(e);
    UpdateFrame();
}

bool nsParticlesPolygonSpawner::Parse(script_state_t *ss, nsParticlesSpawnerContext *context) {
    _frame.clear();
    if (ps_var_begin(ss, "point")) {
        do {
            nsVec2 point;
            if (ps_var_2f(ss, point)) {
                _frame.push_back({point});
            }
        } while (ps_var_next(ss));
    }

    if (ps_var_begin(ss, "angle")) {
        do {
            float angle[2];
            if (ps_var_2f(ss, angle)) {
                auto p = nsVec2::FromAngle(nsMath::ToRad(angle[0])) * angle[1];
                _frame.push_back({p});
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

void nsParticlesPolygonSpawner::Save(nsScriptSaver *ss, nsParticlesSpawnerContext *context) {
    for (const auto &e: _frame) {
        //ss->VarFloat2("point", e->pos1.GetValue(), nsVec2());
    }
}
