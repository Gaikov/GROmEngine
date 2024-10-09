// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesBehaviour.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "engine/renderer/particles/spawner/ParticlesSpawner.h"
#include "engine/renderer/particles/renderer/ParticlesRenderer.h"
#include "engine/renderer/particles/updater/ParticlesUpdater.h"

class nsParticlesBehaviour {
public:
    int amountPerSecond = 100;

    nsParticlesSpawner::sp_t        spawner;
    nsParticlesRenderer::sp_t       renderer;
    nsParticlesUpdater::sp_t        updater;
};