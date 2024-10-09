// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesSpawnerFactory.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/factory/AbstractFactory.h"
#include "Core/SmartPtr.h"
#include "engine/renderer/particles/spawner/ParticlesSpawner.h"
#include "ParticlesSpawnerContext.h"

class nsParticlesSpawnerFactory : public nsAbstractFactory<nsParticlesSpawner>, nsParticlesSpawnerContext {
public:
    nsParticlesSpawnerFactory();

    nsParticlesSpawner* Parse(script_state_t *ss) override;
    nsParticlesSpawner* ParseList(script_state_t *ss);
};