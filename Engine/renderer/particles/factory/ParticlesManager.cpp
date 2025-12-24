//
// Created by Roman on 9/27/2024.
//

#include "ParticlesManager.h"
#include "Core/Config.h"
#include "renderer/particles/ParticlesPool.h"

nsParticlesBehaviour *nsParticlesManager::AllocateResource(const char *resourceName, int param) {
    return  _factory.LoadBehaviour(resourceName);
}

void nsParticlesManager::FreeResource(nsParticlesBehaviour *item) {
    delete item;
}

bool nsParticlesManager::OnInit() {
    Log::Info("### Initialize particles cache ###");
    nsSubSystem::OnInit();
    nsParticlesPool::Init();

    return true;
}

void nsParticlesManager::OnRelease() {
    Log::Info("### Releasing particles cache ###");

    nsParticlesPool::Release();

    ReleaseAll();
    nsSubSystem::OnRelease();
}



