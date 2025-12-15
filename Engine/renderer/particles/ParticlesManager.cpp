//
// Created by Roman on 9/27/2024.
//

#include "ParticlesManager.h"
#include "Core/ParserUtils.h"
#include "Core/ParseFile.h"
#include "Core/Config.h"
#include "ParticlesPool.h"

nsParticlesBehaviour *nsParticlesManager::AllocateResource(const char *resourceName, int param) {
    const auto res = new nsParticlesBehaviour();
    if (LoadBehaviour(resourceName, res)) {
        return res;
    }

    delete res;
    return  nullptr;
}

bool nsParticlesManager::LoadBehaviour(const char *fileName, nsParticlesBehaviour *behaviour) {
    if (!StrCheck(fileName)) {
        Log::Warning("Invalid particles file path");
        return false;
    }
    Log::Info("...loading particles behaviour: %s", fileName);

    nsParseFile pf;
    auto ss = pf.BeginFile(fileName);
    if (!ss) {
        return false;
    }

    auto spawners = _spawnerFactory.ParseList(ss);
    if (!spawners) {
        return false;
    }

    behaviour->amountPerSecond = static_cast<int>(ParseFloat(ss, "amountPerSecond", 100));
    behaviour->spawnTime = ParseFloat(ss, "spawnTime", 0);
    behaviour->spawner = spawners;
    behaviour->updater = _updaterFactory.ParseList(ss);
    behaviour->renderer = _rendererFactory.Parse(ss);

    return true;
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



