// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesToonRenderer.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "ParticlesRenderer.h"
#include "RenDevice.h"
#include "renderer/QuadsBuffer.h"

class nsParticlesToonRenderer : public nsParticlesRenderer {
public:
    static constexpr auto NAME = "toon";

    nsParticlesToonRenderer();

    bool Parse(script_state_t *ss) override;
    void Draw(nsParticle *head) override;

private:
    ITexture    *_front = nullptr;
    ITexture    *_back = nullptr;
    IRenState   *_state = nullptr;
    float       _borderSize = 0;
    nsColor     _borderColor;
    nsQuadsBuffer _buffer;

    void DrawPass(nsParticle *head, float addSize);
};