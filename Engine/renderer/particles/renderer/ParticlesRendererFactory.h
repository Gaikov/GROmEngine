// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesRendererFactory.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "ParticlesRenderer.h"
#include "nsLib/factory/AbstractFactory.h"
#include "Core/Parser.h"

class nsParticlesRendererFactory : public nsAbstractFactory<nsParticlesRenderer> {
public:
    nsParticlesRendererFactory();

    nsParticlesRenderer *Parse(script_state_t *ss, const nsVisualAssetsContext* context);
};
