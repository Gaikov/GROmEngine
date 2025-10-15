// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file GLProgram.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "GLCommon.h"
#include "GLSLCache.h"

class nsGLProgram final {
public:
    explicit nsGLProgram(nsGLSLCache &cache);
    void Init(const char *vertexShader, const char *fragmentShader);

    bool Load();
    void Unload();
    [[nodiscard]] bool Bind();

    bool SetProjView(const float *matrix) const;
    bool SetModel(const float *matrix) const;
    void SetTextureMatrix(const float *matrix) const;
    void SetAlphaCutoff(float cutoff) const;
    void SetHasTexture(bool hasTexture) const;

private:
    std::string _vertexShader;
    std::string _fragmentShader;
    GLuint _program = 0;

    GLint _projView = 0;
    GLint _model = 0;
    GLint _texMat = 0;
    GLint _alphaCutoff = 0;
    GLint _hasTexture = 0;

    nsGLSLCache &_codeCache;

    bool GetUniformLocation(const char *name, GLint &u) const;
};