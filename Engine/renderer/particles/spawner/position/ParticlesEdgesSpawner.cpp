//
// Created by Roman on 10/1/2024.
//

#include "ParticlesEdgesSpawner.h"
#include "Core/ParserUtils.h"
#include "nsLib/log.h"

nsParticlesEdgesSpawner::nsParticlesEdgesSpawner() {
    _name = NAME;
    _title = TITLE;

    points.AddHandler(nsArrayEvent::ID, [this](const nsBaseEvent *e) {
        _valid = false;
        for (const auto &p : points.GetItems()) {
            p->owner = this;
        }
    });
}

nsParticlesEdgesSpawner::~nsParticlesEdgesSpawner() {
    for (const auto &p : points.GetItems()) {
        p->owner = nullptr;
    }
}

void nsParticlesEdgesSpawner::Spawn(nsParticle *p, const float angle) {
    Validate();

    if (!_frame.empty()) {
        float dist = nsMath::Random() * _length * 0.999f;

        for (const auto &e: _frame) {
            dist -= e.length;
            if (dist < 0) {
                p->pos = (e.pos + e.dir * (e.length + dist)).Rotate(angle);
                break;
            }
        }
    }
}

bool nsParticlesEdgesSpawner::Parse(script_state_t *ss, nsParticlesSpawnerContext *context) {
    if (ps_block_begin(ss, "edge")) {
        do {
            nsVec2 p1, p2;
            if (ParseFloat2(ss, "point1", p1)
                && ParseFloat2(ss, "point2", p2)) {
                points.Add(std::make_shared<nsPoint>(p1));
                points.Add(std::make_shared<nsPoint>(p2));
            }
        } while (ps_block_next(ss));
        ps_block_end(ss);
    }

    Validate();

    if (_frame.empty()) {
        Log::Warning("Edges spawner is empty!");
        return false;
    }

    return true;
}

void nsParticlesEdgesSpawner::Save(nsScriptSaver *ss, nsParticlesSpawnerContext *context) {
    auto &list = points.GetItems();
    const auto numEdges = list.size() / 2;

    for (int i = 0; i < numEdges; ++i) {
        if (ss->BlockBegin("edge")) {
            auto &p1 = list[i * 2];
            auto &p2 = list[i * 2 + 1];
            ss->VarFloat2("point1", p1->GetValue(), nsVec2());
            ss->VarFloat2("point2", p2->GetValue(), nsVec2());
            ss->BlockEnd();
        }
    }
}

void nsParticlesEdgesSpawner::Validate() {
    if (!_valid) {
        _valid = true;

        _frame.clear();
        _length = 0;

        const auto &list = points.GetItems();
        const int count = list.size() / 2;
        for (int i = 0; i < count; ++i) {
            auto &p1 = list[i * 2];
            auto &p2 = list[i * 2 + 1];
            Edge e;
            e.pos = p1->GetValue();
            e.dir = p2->GetValue() - e.pos;
            e.length = e.dir.Length();
            e.dir /= e.length;

            _frame.push_back(e);
            _length += e.length;
        }
    }
}

void nsParticlesEdgesSpawner::OnPointChanged(const nsPoint &point) {
    _valid = false;
}
