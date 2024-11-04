// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesBehaviour.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/renderer/particles/spawner/ParticlesSpawner.h"
#include "Engine/renderer/particles/renderer/ParticlesRenderer.h"
#include "Engine/renderer/particles/updater/ParticlesUpdater.h"

class nsParticlesBehaviour {
public:
    int amountPerSecond = 100;
    float spawnTime = 0;

    nsParticlesSpawner::sp_t        spawner;
    nsParticlesRenderer::sp_t       renderer;
    nsParticlesUpdater::sp_t        updater;
};