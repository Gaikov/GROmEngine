// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file GLSLCache.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "GLSLCode.h"
#include "nsLib/factory/ResourcesCache.h"

// GLSL cache that compiles and caches shader objects (GLuint) by file path.
// Shader type is determined by file extension:
//  - vertex:   .vert, .vs, .vsh
//  - fragment: .frag, .fs, .fsh


class nsGLSLCache final : public ResourcesCache<nsGLSLCode, int>
{
public:
    void Invalidate() const;

protected:
    nsGLSLCode *AllocateResource(const char *resourceName, int param) override;
    void FreeResource(nsGLSLCode *item) override;
};
