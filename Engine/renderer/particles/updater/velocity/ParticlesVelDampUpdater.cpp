//
// Created by Roman on 10/29/2024.
//

#include "ParticlesVelDampUpdater.h"
#include "Core/ParserUtils.h"

void nsParticlesVelDampUpdater::Update(nsParticle *p, float deltaTime) {
    float len = p->vel.Length();
    if (len > 0) {
        float newLen = len - deltaTime * _value;
        if (newLen < 0) {
            newLen = 0;
        }
        p->vel = p->vel / len * newLen;
    }
}

bool nsParticlesVelDampUpdater::Parse(script_state_t *ss, nsParticlesUpdaterContext *context) {
    _value = ParseFloat(ss, "value", _value);

    return true;
}
