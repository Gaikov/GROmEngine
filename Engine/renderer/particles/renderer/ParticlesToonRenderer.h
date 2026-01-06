// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesToonRenderer.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "ParticlesRenderer.h"
#include "Engine/RenDevice.h"
#include "Engine/renderer/QuadsBuffer.h"

class nsParticlesToonRenderer : public nsParticlesRenderer {
public:
    static constexpr auto NAME = "toon";

    ITexture    *frontTexture = nullptr;
    ITexture    *backTexture = nullptr;
    IRenState   *renState = nullptr;
    float       borderSize = 0;
    nsColor     frontColor;
    nsColor     backColor;

    nsParticlesToonRenderer();

    bool Parse(script_state_t *ss, const nsVisualAssetsContext *context) override;
    void Save(const nsScriptSaver &saver, const nsVisualAssetsContext *context) override;
    void Draw(nsParticle *head) override;

private:
    nsQuadsBuffer _buffer;

    void DrawPass(nsParticle *head, float addSize);
};