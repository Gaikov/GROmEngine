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

    nsParticlesToonRenderer();

    bool Parse(script_state_t *ss, const nsVisualAssetsContext *context) override;
    void Save(const nsScriptSaver &saver, const nsVisualAssetsContext *context) override;
    void Draw(nsParticle *head) override;

private:
    ITexture    *_front = nullptr;
    ITexture    *_back = nullptr;
    IRenState   *_state = nullptr;
    float       _borderSize = 0;
    nsColor     _color;
    nsColor     _borderColor;
    nsQuadsBuffer _buffer;

    void DrawPass(nsParticle *head, float addSize);
};