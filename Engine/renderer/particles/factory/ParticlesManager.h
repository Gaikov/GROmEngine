// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesManager.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/SubSystem.h"
#include "nsLib/factory/ResourcesCache.h"
#include "ParticlesFactory.h"
#include "ParticlesLoader.h"

class nsParticlesManager : public nsSubSystem<nsParticlesManager>, public ResourcesCache<nsParticlesBehaviour, int>, public nsParticlesLoader {
public:
    nsParticlesBehaviour* LoadParticles(const char *fileName) override {
        return GetResource(fileName, 0);
    }

    const char* GetParticlesPath(nsParticlesBehaviour* particles) override {
        return GetResourceName(particles);
    }

    void SetAssetsContext(const nsVisualAssetsContext::sp_t &context) {
        _factory.assetsContext = context;
    }

protected:
    bool OnInit() override;
    void OnRelease() override;

    nsParticlesBehaviour *AllocateResource(const char *resourceName, int param) override;
    void FreeResource(nsParticlesBehaviour *item) override;

private:
    nsParticlesFactory _factory;
};
