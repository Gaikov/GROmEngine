// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file GLStencilState.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "GLCommon.h"

class nsGLStencilState : public IStencilState {
public:
    static nsGLStencilState* Load(const char *fileName);
    ~nsGLStencilState() override;
    void SetStencilRef(byte value) override;

    bool Reload();
    void Apply(nsGLStencilState *prev) const;
    void ForceApply() const;

private:
    nsString    _fileName;

    bool _enabled = false;

    GLenum  _opFail = GL_KEEP;
    GLenum  _opZFail = GL_KEEP;
    GLenum  _opZPass = GL_KEEP;

    GLenum  _func = GL_ALWAYS;
    GLint   _funcRef = 0;
    mutable bool    _refChanged = true;
    GLuint  _funcMask = 0xff;

    GLuint  _stencilMask = 0xff;

    GLboolean _writeRed = true;
    GLboolean _writeGreen = true;
    GLboolean _writeBlue = true;
    GLboolean _writeAlpha = true;
};