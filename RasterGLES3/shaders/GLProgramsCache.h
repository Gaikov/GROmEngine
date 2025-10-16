// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file GLProgramsCache.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "GLProgram.h"

class nsGLProgramsCache final {
public:
    static constexpr auto DEFAULT_VERTEX_SHADER = "default/rs/gles3/vertex.vert";
    static constexpr auto DEFAULT_FRAGMENT_SHADER = "default/rs/gles3/pixel.frag";

    void Release();
    bool Init();

    nsGLProgram* GetProgram(const char *vertexShaderPath, const char *fragmentShaderPath);
    void Invalidate() const;
    void Bind(nsGLProgram *program, bool force);

    void SetTextureBound(bool hasBound) const;
    void SetTextureMatrix(const nsMatrix & m) const;
    void SetAlphaCutoff(float alphaRef) const;
    void SetProjView(const float *m) const;
    void SetModel(const float *m) const;

private:
    nsGLSLCache _codeCache;
    std::map<std::string, nsGLProgram*> _cache;
    nsGLProgram *_defaultProgram = nullptr;
    nsGLProgram *_currentProgram = nullptr;
};
