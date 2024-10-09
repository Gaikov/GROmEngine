//
// Created by Roman on 10/1/2024.
//

#include "ParticlesPool.h"
#include "nsLib/log.h"

void nsParticlesPool::Reserve(int amount) {
    Log::Info("particles reserved amount: %i/%i", amount, amount + _capacity);
    _capacity += amount;

    for (auto i = 0; i < amount; i++) {
        auto p = new nsParticle();
        p->next = _head;
        _head = p;
    }
}

bool nsParticlesPool::OnInit() {
    nsSubSystem::OnInit();

    Reserve(500);
    return true;
}

void nsParticlesPool::OnRelease() {
    int amount = 0;
    auto p = _head;
    while (p) {
        auto next = p->next;
        delete p;
        p = next;
        amount ++;
    }

    Log::Info("destroyed particles: %i", amount);

    if (amount != _capacity) {
        Log::Error("Particles leaks detected!");
    }

    nsSubSystem::OnRelease();
}

void nsParticlesPool::FreeList(nsParticle *head) {
    while (head) {
        auto next = head->next;
        Free(head);
        head = next;
    }
}

