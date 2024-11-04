// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticleSystem.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/Vec2.h"
#include "Particle.h"
#include "ParticlesBehaviour.h"
#include "ParticlesPool.h"

class nsParticleSystem final {
public:
    bool spawnEnabled = true;
    nsParticlesBehaviour *behaviour;

    explicit nsParticleSystem();
    ~nsParticleSystem();

    bool IsActive() { return _active; }
    void Emit(int amount = 0);

    void SetPosition(const nsVec2 &pos);
    void MoveTo(const nsVec2 &pos);

    void SetRotation(float angle);
    void RotateTo(float angle);

    int Update(float deltaTime);
    void Draw() const;

private:
    nsVec2  _prevPos;
    nsVec2  _currPos;
    float   _prevAngle = 0;
    float   _currAngle = 0;

    nsParticle              *_active = nullptr;
    nsParticlesPool         *_pool;

    float _timeRest = 0;
    float _emitTime = 0;
};