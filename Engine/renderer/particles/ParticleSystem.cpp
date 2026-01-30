//
// Created by Roman on 9/25/2024.
//

#include "ParticleSystem.h"
#include "nsLib/log.h"
#include "Core/Var.h"
#include "spawner/ParticlesLifeSpawner.h"
#include "spawner/ParticlesMultiSpawner.h"

extern nsVar   *r_draw_particles;

nsParticleSystem::nsParticleSystem() :
        behaviour(nullptr),
        _pool(nsParticlesPool::Shared()) {
}

nsParticleSystem::~nsParticleSystem() {
    RemoveAll();
}

void nsParticleSystem::PreSpawn() {
    if (behaviour && spawnEnabled) {
        if (behaviour->spawner) {
            nsParticlesSpawner *root = behaviour->spawner;
            if (const auto ms = dynamic_cast<nsParticlesMultiSpawner*>(root)) {
                if (const auto ls = ms->FindSpawner<nsParticlesLifeSpawner>()) {
                    const auto time = (ls->minLifeTime + ls->maxLifeTime) / 2;
                    Update(time);
                }
            }
        }
    }
}

void nsParticleSystem::RemoveAll() {
    _pool->FreeList(_active);
    _active = nullptr;
}

void nsParticleSystem::Reset(const bool spawn) {
    RemoveAll();
    spawnEnabled = spawn;
}

void nsParticleSystem::GetBounds(nsRect &rect) const {
    if (_active) {
        nsVec2 min = _active->pos;
        nsVec2 max = min;
        for (auto p = _active; p; p = p->next) {
            min = nsVec2::Min(min, p->pos);
            max = nsVec2::Max(max, p->pos);
        }
        rect = {min.x, min.y, max.x - min.x, max.y - min.y};
    } else {
        constexpr float size = 20;
        rect = {-size/2, -size/2, size, size};
    }
}

int nsParticleSystem::Update(float deltaTime) {
    if (!behaviour) {
        return 0;
    }

    int activeAmount = 0;
    auto p = _active;
    nsParticle *prev = nullptr;

    while (p) {
        p->Update(deltaTime);

        if (p->timeLeft <= 0) {
            if (prev)
                prev->next = p->next;
            else
                _active = p->next;
            auto next = p->next;
            _pool->Free(p);
            p = next;
        } else {
            behaviour->updater->Update(p, deltaTime);

            prev = p;
            p = p->next;

            activeAmount++;
        }
    }

    if (spawnEnabled) {
        _emitTime += deltaTime;
        if (behaviour->spawnTime > 0) {
            spawnEnabled = _emitTime < behaviour->spawnTime;
        }

        if (!behaviour->spawner) {
            Log::Warning("Particles spawner is not specified!");
        } else if (spawnEnabled) {
            auto frameTime = deltaTime + _timeRest;
            auto perSecond = (float) behaviour->amountPerSecond;
            auto amount = int(frameTime * perSecond);
            auto amountDeltaTime = perSecond > 0 ? float(amount) / perSecond : 0;
            _timeRest = frameTime - amountDeltaTime;

            nsVec2 pos;
            for (auto i = 0; i < amount; i++) {
                float t = float(i) / float(amount);
                auto p = _pool->Allocate();
                float singleDeltaTime = amountDeltaTime * t;

                behaviour->spawner->Spawn(p, nsMath::Lerp(_currAngle, _prevAngle, t));
                p->Update(singleDeltaTime);

                if (p->timeLeft > 0) {
                    p->next = _active;
                    _active = p;

                    pos.FromLerp(_currPos, _prevPos, t);
                    p->pos += pos;

                    behaviour->updater->Update(p, singleDeltaTime);
                    activeAmount++;
                } else {
                    _pool->Free(p);
                }
            }
        }
    }

    _prevPos = _currPos;
    _prevAngle = _currAngle;
    return activeAmount;
}

void nsParticleSystem::Emit(int amount) {
    if (!behaviour) {
        Log::Warning("Particles behaviour is not specified!");
        return;
    }

    if (!behaviour->spawner) {
        Log::Warning("Particles spawner is not specified!");
        return;
    }

    _emitTime = 0;
    if (!amount) {
        if (behaviour->spawnTime > 0) {
            spawnEnabled = true;
        } else {
            amount = behaviour->amountPerSecond;
        }
    }

    for (auto i = 0; i < amount; i++) {
        auto p = _pool->Allocate();
        behaviour->spawner->Spawn(p, _currAngle);
        p->pos += _currPos;
        behaviour->updater->Update(p, 0);
        p->next = _active;
        _active = p;
    }
}

void nsParticleSystem::Draw() const {
    if (!r_draw_particles->Bool()) {
        return;
    }

    if (behaviour && behaviour->renderer) {
        behaviour->renderer->Draw(_active);
    }
}

void nsParticleSystem::SetPosition(const nsVec2 &pos) {
    _prevPos = _currPos = pos;
}

void nsParticleSystem::MoveTo(const nsVec2 &pos) {
    _prevPos = _currPos;
    _currPos = pos;
}

void nsParticleSystem::SetRotation(float angle) {
    _prevAngle = _currAngle = angle;
}

void nsParticleSystem::RotateTo(const float angle) {
    _prevAngle = nsMath::NormalizeAngle(_currAngle);
    _currAngle = nsMath::ClosestAngle(_prevAngle, angle);
}