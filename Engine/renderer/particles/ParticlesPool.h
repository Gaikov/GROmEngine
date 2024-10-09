// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesPool.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/SubSystem.h"
#include "Particle.h"

class nsParticlesPool : public nsSubSystem<nsParticlesPool> {
public:
    int GetCapacity() const { return _capacity; }

    inline nsParticle* Allocate() {
        if (!_head) {
            Reserve(_capacity);
        }
        auto res = _head;
        res->Reset();
        _head = res->next;
        res->next = nullptr;
        return res;
    }

    inline void Free(nsParticle *p) {
        p->next = _head;
        _head = p;
    }

    void FreeList(nsParticle *head);

protected:
    bool OnInit() override;
    void OnRelease() override;

private:
    nsParticle *_head = nullptr;
    int _capacity = 0;

    void Reserve(int amount);
};