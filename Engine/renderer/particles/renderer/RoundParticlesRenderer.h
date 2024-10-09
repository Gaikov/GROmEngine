// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file RoundParticlesRenderer.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "ParticlesRenderer.h"
#include "engine/RenDevice.h"
#include "engine/renderer/QuadsBuffer.h"

class nsParticlesRoundRenderer : public nsParticlesRenderer {
public:
    static constexpr auto NAME = "round";

    nsParticlesRoundRenderer();
    ~nsParticlesRoundRenderer() override;

    ITexture    *texture = nullptr;
    IRenState   *renState = nullptr;

    void Draw(nsParticle *head) override;

    bool Parse(script_state_t *ss) override;

private:
    nsQuadsBuffer *_buffer;
};