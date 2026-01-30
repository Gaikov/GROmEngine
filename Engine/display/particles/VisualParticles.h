// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file VisualParticles.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/display/VisualType.h"
#include "Engine/display/VisualObject2d.h"
#include "Engine/renderer/particles/ParticleSystem.h"

class nsVisualParticles : public nsVisualObject2d {
public:
    enum Space {
        LOCAL,
        GLOBAL,
        PARENT
    };

    nsProperty<Space> space = LOCAL;
    nsProperty<bool> preSpawn = false;

    nsVisualParticles();

    nsParticleSystem& GetSystem() {
        return _system;
    }

    const char * GetType() override { return nsVisualType::PARTICLES; }

    void ResetPosition();
    void GetLocalBounds(nsRect &bounds) override;
    void Loop() override;
    void DrawContent(const nsVisualContext2d &context) override;
    void ApplyWorldMatrix() override;

protected:
    void OnAddedToStage() override;
    void OnRemovedFromStage() override;

private:
    nsParticleSystem    _system;
};