// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesOverlay.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/SubSystem.h"
#include "ParticlesEffectHolder.h"

class nsParticlesOverlay : public nsSubSystem<nsParticlesOverlay> {
public:
    nsVisualObject2d* GetLayer() { return _layer; }
    nsParticlesEffect* Create(const char *path);
    void CreateBlast(const char *path, const nsVec2 &pos);

protected:
    bool OnInit() override;
    void OnRelease() override;

private:
    nsParticlesEffectHolder   *_layer = nullptr;
};