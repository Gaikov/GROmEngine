// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file GLSLCode.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "GLCommon.h"

class nsGLSLCode final {
public:
    explicit nsGLSLCode(const char *filePath);
    ~nsGLSLCode();
    [[nodiscard]] GLuint GetShader();
    void Invalidate();

private:
    nsString resourceName;
    GLuint _shader = 0;

    void Load();
    void Unload();
};
