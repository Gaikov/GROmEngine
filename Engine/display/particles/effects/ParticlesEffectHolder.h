// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesEffectContainer.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "ParticlesEffect.h"
#include "Engine/display/container/VisualContainer2d.h"

class nsParticlesEffectHolder final : public nsVisualContainer2d  {
public:
    nsParticlesEffectHolder();
    ~nsParticlesEffectHolder() override;

    nsParticlesEffect* CreateEffect(const char *path);

    void Loop() override;
    void RecycleAll();
    bool HasActiveEffects();

protected:
    void OnRemovedFromStage() override;

private:
    std::vector<nsParticlesEffect*> _active;
    std::vector<nsParticlesEffect*> _inactive;
};
