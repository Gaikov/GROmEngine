// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesUpdatersFactory.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/factory/AbstractFactory.h"
#include "GROmEngine/Engine/renderer/particles/updater/ParticlesUpdater.h"
#include "Core/Parser.h"
#include "ParticlesUpdaterContext.h"

class nsParticlesUpdaterFactory : public nsAbstractFactory<nsParticlesUpdater>, public nsParticlesUpdaterContext {
public:
    nsParticlesUpdaterFactory();
    nsParticlesUpdater* Parse(script_state_t *ss);
    nsParticlesUpdater* ParseList(script_state_t *ss);
};