// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesSpawnerFactory.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/factory/AbstractFactory.h"
#include "Engine/renderer/particles/spawner/ParticlesSpawner.h"
#include "ParticlesSpawnerContext.h"
#include "Core/ScriptSaver.h"

class nsParticlesSpawnerFactory : public nsAbstractFactory<nsParticlesSpawner>, nsParticlesSpawnerContext {
public:
    nsParticlesSpawnerFactory();

    nsParticlesSpawner* Parse(script_state_t *ss) override;
    nsParticlesSpawner* ParseList(script_state_t *ss);
    void Save(nsScriptSaver *ss, nsParticlesSpawner *spawner) override;
};
