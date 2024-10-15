// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file VisualParticles.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "display/VisualObject2d.h"
#include "renderer/particles/ParticleSystem.h"

class nsVisualParticles : public nsVisualObject2d {
public:
    nsParticleSystem& GetSystem() {
        return _system;
    }

    void GetLocalBounds(nsRect &bounds) override;
    void Loop() override;
    void DrawContent(const nsVisualContext2d &context) override;

private:
    nsParticleSystem    _system;
};