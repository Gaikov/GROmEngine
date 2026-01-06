// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesFactory.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/assets/VisualAssetsContext.h"
#include "Engine/renderer/particles/ParticlesBehaviour.h"
#include "Engine/renderer/particles/renderer/ParticlesRendererFactory.h"
#include "Engine/renderer/particles/spawner/factory/ParticlesSpawnerFactory.h"
#include "Engine/renderer/particles/updater/factory/ParticlesUpdaterFactory.h"

class nsParticlesFactory final {
public:
    nsVisualAssetsContext::sp_t assetsContext;
    nsParticlesBehaviour* LoadBehaviour(const char* path);
    bool SaveBehaviour(const nsParticlesBehaviour* behaviour, const char* path);

private:
    nsParticlesSpawnerFactory   _spawnerFactory;
    nsParticlesRendererFactory  _rendererFactory;
    nsParticlesUpdaterFactory   _updaterFactory;
};
