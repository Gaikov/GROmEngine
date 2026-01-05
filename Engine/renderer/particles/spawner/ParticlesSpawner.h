// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file IParticlesSpawnerPolicy.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/renderer/particles/Particle.h"
#include "Core/SmartPtr.h"
#include "Core/Parser.h"
#include "Core/ScriptSaver.h"

class nsParticlesSpawnerContext;

class nsParticlesSpawner {
public:
    typedef nsSmartPtr<nsParticlesSpawner>   sp_t;

    virtual ~nsParticlesSpawner() = default;
    virtual void Spawn(nsParticle *p, float angle) = 0;

    virtual bool Parse(script_state_t *ss, nsParticlesSpawnerContext *context) = 0;
    virtual void Save(nsScriptSaver *ss, nsParticlesSpawnerContext *context) = 0;

    [[nodiscard]] const char *GetName() const { return _name; }
    [[nodiscard]] const char *GetTitle() const { return _title; }

protected:
    const char *_title = "";
    const char *_name = "";
};