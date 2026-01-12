//
// Created by Roman on 11/4/2024.
//

#include "ParticlesMultiDirectionSpawner.h"
#include "nsLib/log.h"
#include "Core/ParserUtils.h"

void nsParticlesMultiDirectionSpawner::Spawn(nsParticle *p, const float angle) {
    if (directions.empty()) {
        return;
    }

    const int dirIndex = int(nsMath::Random() * 0.999f * (float)directions.size());
    const auto &dir = directions[dirIndex];

    const float partAngle = nsMath::Lerp(dir->minAngle, dir->maxAngle, nsMath::Random());
    const float speed = nsMath::Lerp(dir->minSpeed, dir->maxSpeed, nsMath::Random());

    const auto vel = nsVec2::FromAngle(partAngle) * speed;

    p->vel = vel.Rotate(angle);
}

bool nsParticlesMultiDirectionSpawner::Parse(script_state_t *ss, nsParticlesSpawnerContext *context) {

    if (ps_block_begin(ss, "direction")) {
        do {
            auto dir = std::make_shared<Direction>();
            dir->minAngle = nsMath::ToRad(ParseFloat(ss, "minAngle"));
            dir->maxAngle = nsMath::ToRad(ParseFloat(ss, "maxAngle"));

            dir->minSpeed = ParseFloat(ss, "minSpeed");
            dir->maxSpeed = ParseFloat(ss, "maxSpeed");

            directions.push_back(dir);
        } while (ps_block_next(ss));
        ps_block_end(ss);
    }

    if (directions.empty()) {
        Log::Warning("no multiple directions parsed!");
        return false;
    }

    return true;
}

void nsParticlesMultiDirectionSpawner::Save(nsScriptSaver *ss, nsParticlesSpawnerContext *context) {
    for (const auto &dir: directions) {
        if (ss->BlockBegin("direction")) {
            ss->VarFloat("minAngle", nsMath::ToDeg(dir->minAngle), 0);
            ss->VarFloat("maxAngle", nsMath::ToDeg(dir->maxAngle), 0);
            ss->VarFloat("minSpeed", dir->minSpeed, 0);
            ss->VarFloat("maxSpeed", dir->maxSpeed, 0);
            ss->BlockEnd();
        }
    }
}