// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesEffect.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/display/factory/CustomVisual.h"
#include "Engine/display/container/VisualContainer2d.h"
#include "Engine/display/particles/VisualParticles.h"
#include "Engine/display/pool/CachedVisual.h"

class nsParticlesEffect : public nsVisualContainer2d, public nsCachedVisual, public nsCustomVisual {
public:
    const char *GetLayoutPath() { return _path.c_str(); }

    void Emit(bool value);
    bool IsActive();
    void Blast();
    void SetPosition(const nsVec2 &pos);
    void Reset(bool emit);

    bool ParseCustomProps(script_state_t *ss) override;

protected:
    void OnVisualCreated(const char *path) override;
    void OnVisualPrepare() override;
    void OnVisualRecycled() override;

private:
    std::string _path;
    std::vector<nsVisualParticles*> _particles;
};
