//
// Created by Roman on 11/4/2024.
//

#include "ParticlesMultiDirectionSpawner.h"
#include "nsLib/log.h"
#include "Core/ParserUtils.h"

void nsParticlesMultiDirectionSpawner::Spawn(nsParticle *p, float angle) {
    int dirIndex = int(nsMath::Random() * 0.999f * (float)_directions.size());
    auto &dir = _directions[dirIndex];

    float partAngle = nsMath::Lerp(dir.minAngle, dir.maxAngle, nsMath::Random());
    float speed = nsMath::Lerp(dir.minSpeed, dir.maxSpeed, nsMath::Random());

    auto vel = nsVec2::FromAngle(partAngle) * speed;
    vel.Rotate(angle);

    p->vel = vel;
}

bool nsParticlesMultiDirectionSpawner::Parse(script_state_t *ss, nsParticlesSpawnerContext *context) {

    if (ps_block_begin(ss, "direction")) {
        do {
            Direction   dir;
            dir.minAngle = nsMath::ToRad(ParseFloat(ss, "minAngle"));
            dir.maxAngle = nsMath::ToRad(ParseFloat(ss, "maxAngle"));
            dir.minAngle = nsMath::NormalizeAngle(dir.minAngle);
            dir.maxAngle = nsMath::ClosestAngle(dir.minAngle, dir.maxAngle);

            dir.minSpeed = ParseFloat(ss, "minSpeed");
            dir.maxSpeed = ParseFloat(ss, "maxSpeed");

            _directions.push_back(dir);
        } while (ps_block_next(ss));
        ps_block_end(ss);
    }

    if (_directions.empty()) {
        Log::Warning("no multiple directions parsed!");
        return false;
    }

    return true;
}
