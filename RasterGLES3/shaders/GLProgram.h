// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file GLProgram.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "GLCommon.h"
#include "GLSLCache.h"
#include <map>

class nsGLProgram;

class GLUniform : public IShaderUniform {
public:
    GLUniform(nsGLProgram *program, const char *name);

    void SetFloat(float value) override;
    void SetFloat3(const float value[3]) override;
    void SetFloat4(const float value[4]) override;

    void ResolveAndApply();

    void Apply();

private:
    void Set(const float *value, int count);

    nsGLProgram *_program;
    std::string  _name;
    GLint        _location = -1;
    float        _value[4] = {};
    int          _count    = 0;
};

class nsGLProgram final {
public:
    explicit nsGLProgram(nsGLSLCache &cache);
    ~nsGLProgram();

    void Init(const char *vertexShader, const char *fragmentShader);

    bool Load();
    void Unload();
    bool Bind();

    void SetProjView(const float *matrix) const;
    void SetModel(const float *matrix) const;
    void SetTextureMatrix(const float *matrix) const;
    void SetAlphaCutoff(float cutoff) const;
    void SetHasTexture(bool hasTexture) const;

    IShaderUniform *GetUniform(const char *name);
    GLuint GetGLProgram() const { return _program; }
    void ResolveUserUniforms();
    void ApplyUserUniforms();

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

    std::map<std::string, IShaderUniform*> _userUniforms;

    bool GetUniformLocation(const char *name, GLint &u) const;
};