// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file GLStencilManager.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/factory/ResourcesCache.h"
#include "GLStencilState.h"

class nsGLStencilManager : public ResourcesCache<nsGLStencilState, int> {
public:
    void Init();
    void Apply(nsGLStencilState *state);

protected:
    nsGLStencilState *AllocateResource(const char *id, int param) override;
    void FreeResource(nsGLStencilState *item) override;

private:
    nsGLStencilState    _default;
    nsGLStencilState    *_currState = &_default;
};