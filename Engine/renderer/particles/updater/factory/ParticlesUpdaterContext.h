// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesUpdaterContext.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/renderer/particles/updater/ParticlesUpdater.h"
#include "Core/Parser.h"
#include "Core/ScriptSaver.h"

class nsParticlesUpdaterContext {
public:
    virtual ~nsParticlesUpdaterContext() = default;
    virtual nsParticlesUpdater* Parse(script_state_t *ss) = 0;
    virtual void Save(nsScriptSaver *ss, nsParticlesUpdater *updater) = 0;
};
