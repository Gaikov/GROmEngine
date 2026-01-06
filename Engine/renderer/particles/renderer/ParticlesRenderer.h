// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesRenderer.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/renderer/particles/Particle.h"
#include "Core/SmartPtr.h"
#include "Core/Parser.h"
#include "Core/ScriptSaver.h"

class nsVisualAssetsContext;

class nsParticlesRenderer {
public:
    typedef nsSmartPtr<nsParticlesRenderer> sp_t;

    virtual ~nsParticlesRenderer() = default;
    virtual bool Parse(script_state_t *ss, const nsVisualAssetsContext *context) = 0;
    virtual void Draw(nsParticle *head) = 0;
    [[nodiscard]] const char* GetName() const { return _name; }
    virtual void Save(const nsScriptSaver & saver, const nsVisualAssetsContext * context) = 0;

protected:
    const char *_name = "";
};