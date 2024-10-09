// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Particle.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/Vec3.h"
#include "nsLib/color.h"

struct nsParticle {
    nsVec3  pos;
    nsVec3  vel;
    nsColor color;

    float   startSize = 0;
    float   size = 0;

    float   angle = 0;

    float   timeLeft = 0;
    float   maxTime = 0;
    float   timeLine = 0; //life timeline from 0 to 1

    nsParticle  *next = nullptr;

    inline void Reset() {
        pos = {0, 0};
        vel = {0, 0};
        color = nsColor::white;
        startSize = size = 0;
        angle = 0;
        timeLine = timeLeft = maxTime = 0;
    }

    inline void Update(float deltaTime) {
        timeLeft -= deltaTime;
        timeLine = 1 - timeLeft / maxTime;
    }
};