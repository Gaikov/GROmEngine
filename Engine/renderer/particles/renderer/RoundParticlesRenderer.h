// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file RoundParticlesRenderer.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "ParticlesRenderer.h"
#include "Engine/RenDevice.h"
#include "Engine/renderer/QuadsBuffer.h"

class nsParticlesRoundRenderer : public nsParticlesRenderer {
public:
    static constexpr auto NAME = "round";

    nsParticlesRoundRenderer();
    ~nsParticlesRoundRenderer() override;

    ITexture    *texture = nullptr;
    IRenState   *renState = nullptr;

    void Draw(nsParticle *head) override;

    bool Parse(script_state_t *ss, const nsVisualAssetsContext *context) override;
    void Save(const nsScriptSaver &saver, const nsVisualAssetsContext *context) override;

private:
    nsQuadsBuffer *_buffer;
};