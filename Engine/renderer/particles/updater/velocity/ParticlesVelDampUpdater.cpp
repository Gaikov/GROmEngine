//
// Created by Roman on 10/29/2024.
//

#include "ParticlesVelDampUpdater.h"
#include "Core/ParserUtils.h"

void nsParticlesVelDampUpdater::Update(nsParticle *p, float deltaTime) {
    if (const float len = p->vel.Length(); len > 0) {
        float newLen = len - deltaTime * value;
        if (newLen < 0) {
            newLen = 0;
        }
        p->vel = p->vel / len * newLen;
    }
}

bool nsParticlesVelDampUpdater::Parse(script_state_t *ss, nsParticlesUpdaterContext *context) {
    value = ParseFloat(ss, "value", value);

    return true;
}

void nsParticlesVelDampUpdater::Save(nsScriptSaver *ss, nsParticlesUpdaterContext *context) {
    ss->VarFloat("value", value, 1);
}
