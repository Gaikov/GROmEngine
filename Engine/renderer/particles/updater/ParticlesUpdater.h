// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesUpdater.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Core/SmartPtr.h"
#include "Core/Parser.h"
#include "Core/ScriptSaver.h"
#include "Engine/renderer/particles/Particle.h"

class nsParticlesUpdaterContext;

class nsParticlesUpdater {
    friend class nsParticlesUpdaterFactory;

public:
    virtual ~nsParticlesUpdater() = default;

    typedef nsSmartPtr<nsParticlesUpdater>  sp_t;

    [[nodiscard]] const char *GetName() const { return _name; }
    const char* GetTitle() const { return _title; }

    virtual void Update(nsParticle *p, float deltaTime) = 0;
    virtual bool Parse(script_state_t *ss, nsParticlesUpdaterContext *context) = 0;
    virtual void Save(nsScriptSaver *ss, nsParticlesUpdaterContext *context) = 0;

protected:
    const char *_name = "";
    const char * _title = "";
};