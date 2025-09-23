// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file GLProgram.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "GLCommon.h"

class nsGLProgram final {
public:
    nsGLProgram(const char *vertexShader, const char *fragmentShader);

    bool Load();
    void Unload();
    [[nodiscard]] bool Bind();

    static GLuint createShader(GLenum type, const char *source);

    bool SetProjView(const float *projView) const;
    bool SetModel(const float *model) const;
    bool SetAlphaCutoff(float cutoff) const;

private:

    std::string _vertexShader;
    std::string _fragmentShader;
    GLuint _program = 0;
    GLuint _vs = 0;
    GLuint _fs = 0;

    GLint _projView = 0;
    GLint _model = 0;
    GLint _alphaCutoff = 0;

    bool GetUniformLocation(const char *name, GLint &u) const;
};