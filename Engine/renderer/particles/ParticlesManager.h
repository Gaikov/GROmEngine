// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesManager.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/SubSystem.h"
#include "nsLib/factory/ResourcesCache.h"
#include "ParticlesBehaviour.h"
#include "Engine/renderer/particles/spawner/factory/ParticlesSpawnerFactory.h"
#include "Engine/renderer/particles/renderer/ParticlesRendererFactory.h"
#include "Engine/renderer/particles/updater/factory/ParticlesUpdaterFactory.h"

class nsParticlesManager : public nsSubSystem<nsParticlesManager>, public ResourcesCache<nsParticlesBehaviour, int> {
public:
    nsParticlesBehaviour* LoadParticles(const char *fileName) {
        return GetResource(fileName, 0);
    }

protected:
    bool OnInit() override;
    void OnRelease() override;

    nsParticlesBehaviour *AllocateResource(const char *resourceName, int param) override;
    void FreeResource(nsParticlesBehaviour *item) override;
    bool LoadBehaviour(const char *fileName, nsParticlesBehaviour *behaviour);
    void ReloadParticles();

    static void ReloadParticles_f(int argc, const char *argv[]);

private:
    nsParticlesSpawnerFactory   _spawnerFactory;
    nsParticlesRendererFactory  _rendererFactory;
    nsParticlesUpdaterFactory   _updaterFactory;
};