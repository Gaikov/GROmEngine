// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file GLProgramsCache.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "GLProgram.h"

class nsGLProgramsCache final {
public:
    nsGLProgram* GetProgram(const char *vertexShaderPath, const char *fragmentShaderPath);
    void Invalidate() const;
    void Release();

private:
    nsGLSLCache _codeCache;
    std::map<std::string, nsGLProgram*> _cache;
};
