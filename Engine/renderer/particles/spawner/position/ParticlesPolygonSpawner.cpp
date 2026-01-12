//
// Created by Roman on 9/26/2024.
//

#include "ParticlesPolygonSpawner.h"
#include "Core/ParserUtils.h"
#include "nsLib/log.h"
#include "renderer/particles/spawner/factory/ParticlesSpawnerContext.h"

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

    Validate();
    return true;
}

void nsParticlesPolygonSpawner::Save(nsScriptSaver *ss, nsParticlesSpawnerContext *context) {
    for (const auto &p: points.GetItems()) {
        const auto pos = p->GetValue();
        ss->PrintVar("point", "%f %f", pos.x, pos.y);
    }
}

void nsParticlesPolygonSpawner::Validate() {
    if (!_valid) {
        _frame.clear();
        _length = 0;

        for (auto &p : points.GetItems()) {
            _frame.push_back({p->GetValue()});
        }

        if (_frame.size() > 1) {
            for (auto i = 0; i < _frame.size(); i++) {
                auto &e1 = _frame[i];
                auto &e2 = _frame[(i + 1) % _frame.size()];

                e1.dir = e2.pos - e1.pos;
                e1.length = e1.dir.Length();
                e1.dir /= e1.length;

                _length += e1.length;
            }
        } else {
            Log::Warning("Polygon spawner has only one point!");
        }
    }
}
